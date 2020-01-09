/*
 * empathy-account-settings.c - Source for EmpathyAccountSettings
 * Copyright (C) 2009 Collabora Ltd.
 * @author Sjoerd Simons <sjoerd.simons@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"

#include "empathy-account-settings.h"
#include "empathy-connection-managers.h"
#include "empathy-keyring.h"
#include "empathy-utils.h"
#include "empathy-presence-manager.h"

#define DEBUG_FLAG EMPATHY_DEBUG_ACCOUNT
#include <libempathy/empathy-debug.h>

#define GET_PRIV(obj) EMPATHY_GET_PRIV (obj, EmpathyAccountSettings)

G_DEFINE_TYPE(EmpathyAccountSettings, empathy_account_settings, G_TYPE_OBJECT)

enum {
  PROP_ACCOUNT = 1,
  PROP_CM_NAME,
  PROP_PROTOCOL,
  PROP_SERVICE,
  PROP_DISPLAY_NAME,
  PROP_DISPLAY_NAME_OVERRIDDEN,
  PROP_READY
};

enum {
  PASSWORD_RETRIEVED = 1,
  LAST_SIGNAL
};

static gulong signals[LAST_SIGNAL] = { 0, };

/* private structure */
typedef struct _EmpathyAccountSettingsPriv EmpathyAccountSettingsPriv;

struct _EmpathyAccountSettingsPriv
{
  gboolean dispose_has_run;
  EmpathyConnectionManagers *managers;
  TpAccountManager *account_manager;

  TpConnectionManager *manager;
  TpProtocol *protocol_obj;

  TpAccount *account;
  gchar *cm_name;
  gchar *protocol;
  gchar *service;
  gchar *display_name;
  gchar *icon_name;
  gchar *storage_provider;
  gboolean display_name_overridden;
  gboolean ready;

  gboolean supports_sasl;
  gboolean remember_password;

  gchar *password;
  gchar *password_original;

  gboolean password_retrieved;
  gboolean password_requested;

  /* Parameter name (gchar *) -> parameter value (GVariant) */
  GHashTable *parameters;
  /* Keys are parameter names from the hash above (gchar *).
   * Values are regular expresions that should match corresponding parameter
   * values (GRegex *). Possible regexp patterns are defined in
   * empathy-account-widget.c */
  GHashTable *param_regexps;
  GArray *unset_parameters;
  GList *required_params;

  gulong managers_ready_id;
  gboolean preparing_protocol;

  /* If TRUE, the account should have 'tel' in its
   * Account.Interface.Addressing.URISchemes property. */
  gboolean uri_scheme_tel;
  /* If TRUE, Service property needs to be updated when applying changes */
  gboolean update_service;

  GSimpleAsyncResult *apply_result;
};

static void
empathy_account_settings_init (EmpathyAccountSettings *obj)
{
  EmpathyAccountSettingsPriv *priv = G_TYPE_INSTANCE_GET_PRIVATE ((obj),
    EMPATHY_TYPE_ACCOUNT_SETTINGS, EmpathyAccountSettingsPriv);

  obj->priv = priv;

  /* allocate any data required by the object here */
  priv->managers = empathy_connection_managers_dup_singleton ();
  priv->account_manager = tp_account_manager_dup ();

  priv->parameters = g_hash_table_new_full (g_str_hash, g_str_equal,
    g_free, (GDestroyNotify) g_variant_unref);

  priv->param_regexps = g_hash_table_new_full (g_str_hash, g_str_equal,
    g_free, (GDestroyNotify) g_regex_unref);

  priv->unset_parameters = g_array_new (TRUE, FALSE, sizeof (gchar *));

  priv->required_params = NULL;
}

static void empathy_account_settings_dispose (GObject *object);
static void empathy_account_settings_finalize (GObject *object);
static void empathy_account_settings_account_ready_cb (GObject *source_object,
    GAsyncResult *result, gpointer user_data);
static void empathy_account_settings_managers_ready_cb (GObject *obj,
    GParamSpec *pspec, gpointer user_data);
static void empathy_account_settings_check_readyness (
    EmpathyAccountSettings *self);

static void
empathy_account_settings_set_property (GObject *object,
    guint prop_id,
    const GValue *value,
    GParamSpec *pspec)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (object);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  switch (prop_id)
    {
      case PROP_ACCOUNT:
        priv->account = g_value_dup_object (value);
        break;
      case PROP_CM_NAME:
        priv->cm_name = g_value_dup_string (value);
        break;
      case PROP_PROTOCOL:
        priv->protocol = g_value_dup_string (value);
        break;
      case PROP_SERVICE:
        priv->service = g_value_dup_string (value);
        break;
      case PROP_DISPLAY_NAME:
        priv->display_name = g_value_dup_string (value);
        break;
      case PROP_DISPLAY_NAME_OVERRIDDEN:
        priv->display_name_overridden = g_value_get_boolean (value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
empathy_account_settings_get_property (GObject *object,
    guint prop_id,
    GValue *value,
    GParamSpec *pspec)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (object);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  switch (prop_id)
    {
      case PROP_ACCOUNT:
        g_value_set_object (value, priv->account);
        break;
      case PROP_CM_NAME:
        g_value_set_string (value, priv->cm_name);
        break;
      case PROP_PROTOCOL:
        g_value_set_string (value, priv->protocol);
        break;
      case PROP_SERVICE:
        g_value_set_string (value, priv->service);
        break;
      case PROP_DISPLAY_NAME:
        g_value_set_string (value, priv->display_name);
        break;
      case PROP_DISPLAY_NAME_OVERRIDDEN:
        g_value_set_boolean (value, priv->display_name_overridden);
        break;
      case PROP_READY:
        g_value_set_boolean (value, priv->ready);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
empathy_account_settings_constructed (GObject *object)
{
  EmpathyAccountSettings *self = EMPATHY_ACCOUNT_SETTINGS (object);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  if (priv->account != NULL)
    {
      g_free (priv->cm_name);
      g_free (priv->protocol);
      g_free (priv->service);

      priv->cm_name =
        g_strdup (tp_account_get_cm_name (priv->account));
      priv->protocol =
        g_strdup (tp_account_get_protocol_name (priv->account));
      priv->service =
        g_strdup (tp_account_get_service (priv->account));
      priv->icon_name = g_strdup
        (tp_account_get_icon_name (priv->account));
    }
  else
    {
      priv->icon_name = empathy_protocol_icon_name (priv->protocol);
    }

  g_assert (priv->cm_name != NULL && priv->protocol != NULL);

  empathy_account_settings_check_readyness (self);

  if (!priv->ready)
    {
      GQuark features[] = {
          TP_ACCOUNT_FEATURE_CORE,
          TP_ACCOUNT_FEATURE_STORAGE,
          TP_ACCOUNT_FEATURE_ADDRESSING,
          0 };

      if (priv->account != NULL)
        {
          tp_proxy_prepare_async (priv->account, features,
              empathy_account_settings_account_ready_cb, self);
        }

      tp_g_signal_connect_object (priv->managers, "notify::ready",
        G_CALLBACK (empathy_account_settings_managers_ready_cb), object, 0);
    }

  if (G_OBJECT_CLASS (
        empathy_account_settings_parent_class)->constructed != NULL)
    G_OBJECT_CLASS (
        empathy_account_settings_parent_class)->constructed (object);
}


static void
empathy_account_settings_class_init (
    EmpathyAccountSettingsClass *empathy_account_settings_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (empathy_account_settings_class);

  g_type_class_add_private (empathy_account_settings_class, sizeof
      (EmpathyAccountSettingsPriv));

  object_class->dispose = empathy_account_settings_dispose;
  object_class->finalize = empathy_account_settings_finalize;
  object_class->set_property = empathy_account_settings_set_property;
  object_class->get_property = empathy_account_settings_get_property;
  object_class->constructed = empathy_account_settings_constructed;

  g_object_class_install_property (object_class, PROP_ACCOUNT,
    g_param_spec_object ("account",
      "Account",
      "The TpAccount backing these settings",
      TP_TYPE_ACCOUNT,
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_CM_NAME,
    g_param_spec_string ("connection-manager",
      "connection-manager",
      "The name of the connection manager this account uses",
      NULL,
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_PROTOCOL,
    g_param_spec_string ("protocol",
      "Protocol",
      "The name of the protocol this account uses",
      NULL,
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_SERVICE,
    g_param_spec_string ("service",
      "Service",
      "The service of this account, or NULL",
      NULL,
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_DISPLAY_NAME,
    g_param_spec_string ("display-name",
      "display-name",
      "The display name account these settings belong to",
      NULL,
      G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class, PROP_DISPLAY_NAME_OVERRIDDEN,
      g_param_spec_boolean ("display-name-overridden",
        "display-name-overridden",
        "Whether the display name for this account has been manually "
        "overridden",
        FALSE,
        G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE));

  g_object_class_install_property (object_class, PROP_READY,
    g_param_spec_boolean ("ready",
      "Ready",
      "Whether this account is ready to be used",
      FALSE,
      G_PARAM_STATIC_STRINGS | G_PARAM_READABLE));

  signals[PASSWORD_RETRIEVED] =
      g_signal_new ("password-retrieved",
          G_TYPE_FROM_CLASS (empathy_account_settings_class),
          G_SIGNAL_RUN_LAST, 0, NULL, NULL,
          g_cclosure_marshal_generic,
          G_TYPE_NONE, 0);
}

static void
empathy_account_settings_dispose (GObject *object)
{
  EmpathyAccountSettings *self = EMPATHY_ACCOUNT_SETTINGS (object);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;

  if (priv->managers_ready_id != 0)
    g_signal_handler_disconnect (priv->managers, priv->managers_ready_id);
  priv->managers_ready_id = 0;

  tp_clear_object (&priv->managers);
  tp_clear_object (&priv->manager);
  tp_clear_object (&priv->account_manager);
  tp_clear_object (&priv->account);
  tp_clear_object (&priv->protocol_obj);

  /* release any references held by the object here */
  if (G_OBJECT_CLASS (empathy_account_settings_parent_class)->dispose)
    G_OBJECT_CLASS (empathy_account_settings_parent_class)->dispose (object);
}

static void
empathy_account_settings_free_unset_parameters (
    EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  guint i;

  for (i = 0 ; i < priv->unset_parameters->len; i++)
    g_free (g_array_index (priv->unset_parameters, gchar *, i));

  g_array_set_size (priv->unset_parameters, 0);
}

static void
empathy_account_settings_finalize (GObject *object)
{
  EmpathyAccountSettings *self = EMPATHY_ACCOUNT_SETTINGS (object);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);
  GList *l;

  /* free any data held directly by the object here */
  g_free (priv->cm_name);
  g_free (priv->protocol);
  g_free (priv->service);
  g_free (priv->display_name);
  g_free (priv->icon_name);
  g_free (priv->password);
  g_free (priv->password_original);
  g_free (priv->storage_provider);

  if (priv->required_params != NULL)
    {
      for (l = priv->required_params; l; l = l->next)
        g_free (l->data);
      g_list_free (priv->required_params);
    }

  g_hash_table_unref (priv->parameters);
  g_hash_table_unref (priv->param_regexps);

  empathy_account_settings_free_unset_parameters (self);
  g_array_unref (priv->unset_parameters);

  G_OBJECT_CLASS (empathy_account_settings_parent_class)->finalize (object);
}

static void
empathy_account_settings_protocol_obj_prepared_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyAccountSettings *self = user_data;
  GError *error = NULL;

  if (!tp_proxy_prepare_finish (source, result, &error))
    {
      DEBUG ("Failed to prepare protocol object: %s", error->message);
      g_clear_error (&error);
      return;
    }

  empathy_account_settings_check_readyness (self);
}

static void
empathy_account_settings_get_password_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyAccountSettings *self = user_data;
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);
  const gchar *password;
  GError *error = NULL;

  password = empathy_keyring_get_account_password_finish (TP_ACCOUNT (source),
      result, &error);

  if (error != NULL)
    {
      DEBUG ("Failed to get password: %s", error->message);
      g_clear_error (&error);
    }

  /* It doesn't really matter if getting the password failed; that
   * just means that it's not there, or let's act like that at
   * least. */

  g_assert (priv->password == NULL);

  priv->password = g_strdup (password);
  priv->password_original = g_strdup (password);

  g_signal_emit (self, signals[PASSWORD_RETRIEVED], 0);
}

static GVariant * empathy_account_settings_dup (
    EmpathyAccountSettings *settings,
    const gchar *param);

static void
empathy_account_settings_check_readyness (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);
  GQuark features[] = { TP_PROTOCOL_FEATURE_CORE, 0 };

  if (priv->ready)
    return;

  if (priv->account != NULL
      && !tp_account_is_prepared (priv->account, TP_ACCOUNT_FEATURE_CORE))
      return;

  if (!empathy_connection_managers_is_ready (priv->managers))
    return;

  if (priv->manager == NULL)
    {
      priv->manager = empathy_connection_managers_get_cm (
          priv->managers, priv->cm_name);
    }

  if (priv->manager == NULL)
    return;

  g_object_ref (priv->manager);

  if (priv->account != NULL)
    {
      g_free (priv->display_name);
      priv->display_name =
        g_strdup (tp_account_get_display_name (priv->account));

      g_free (priv->icon_name);
      priv->icon_name =
        g_strdup (tp_account_get_icon_name (priv->account));

      priv->uri_scheme_tel = empathy_account_has_uri_scheme_tel (priv->account);
    }

  if (priv->protocol_obj == NULL)
    {
      priv->protocol_obj = g_object_ref (
          tp_connection_manager_get_protocol_object (priv->manager,
            priv->protocol));
    }

  if (!tp_proxy_is_prepared (priv->protocol_obj, TP_PROTOCOL_FEATURE_CORE)
      && !priv->preparing_protocol)
    {
      priv->preparing_protocol = TRUE;
      tp_proxy_prepare_async (priv->protocol_obj, features,
          empathy_account_settings_protocol_obj_prepared_cb, self);
      return;
    }
  else
    {
      if (tp_strv_contains (tp_protocol_get_authentication_types (
                  priv->protocol_obj),
              TP_IFACE_CHANNEL_INTERFACE_SASL_AUTHENTICATION))
        {
          priv->supports_sasl = TRUE;
        }
    }

  if (priv->required_params == NULL)
    {
      GList *params, *l;

      params = tp_protocol_dup_params (priv->protocol_obj);
      for (l = params; l != NULL; l = g_list_next (l))
        {
          TpConnectionManagerParam *cur = l->data;

          if (tp_connection_manager_param_is_required (cur))
            {
              priv->required_params = g_list_append (priv->required_params,
                  g_strdup (tp_connection_manager_param_get_name (cur)));
            }
        }

       g_list_free_full (params,
           (GDestroyNotify) tp_connection_manager_param_free);
    }

  /* priv->account won't be a proper account if it's the account
   * assistant showing this widget. */
  if (priv->supports_sasl && !priv->password_requested
      && priv->account != NULL)
    {
      priv->password_requested = TRUE;

      /* Make this call but don't block on its readiness. We'll signal
       * if it's updated later with ::password-retrieved. */
      empathy_keyring_get_account_password_async (priv->account,
          empathy_account_settings_get_password_cb, self);
    }

  priv->ready = TRUE;
  g_object_notify (G_OBJECT (self), "ready");
}

static void
empathy_account_settings_account_ready_cb (GObject *source_object,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (user_data);
  TpAccount *account = TP_ACCOUNT (source_object);
  GError *error = NULL;

  if (!tp_proxy_prepare_finish (account, result, &error))
    {
      DEBUG ("Failed to prepare account: %s", error->message);
      g_error_free (error);
      return;
    }

  empathy_account_settings_check_readyness (settings);
}

static void
empathy_account_settings_managers_ready_cb (GObject *object,
    GParamSpec *pspec,
    gpointer user_data)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (user_data);

  empathy_account_settings_check_readyness (settings);
}

EmpathyAccountSettings *
empathy_account_settings_new (const gchar *connection_manager,
    const gchar *protocol,
    const gchar *service,
    const char *display_name)
{
  return g_object_new (EMPATHY_TYPE_ACCOUNT_SETTINGS,
      "connection-manager", connection_manager,
      "protocol", protocol,
      "service", service,
      "display-name", display_name,
      NULL);
}

EmpathyAccountSettings *
empathy_account_settings_new_for_account (TpAccount *account)
{
  return g_object_new (EMPATHY_TYPE_ACCOUNT_SETTINGS,
      "account", account,
      NULL);
}

GList *
empathy_account_settings_dup_tp_params (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  g_return_val_if_fail (priv->protocol_obj != NULL, NULL);

  return tp_protocol_dup_params (priv->protocol_obj);
}

gboolean
empathy_account_settings_is_ready (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->ready;
}

const gchar *
empathy_account_settings_get_cm (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->cm_name;
}

const gchar *
empathy_account_settings_get_protocol (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->protocol;
}

const gchar *
empathy_account_settings_get_service (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->service;
}

void
empathy_account_settings_set_service (EmpathyAccountSettings *settings,
    const gchar *service)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  if (!tp_strdiff (priv->service, service))
    return;

  g_free (priv->service);
  priv->service = g_strdup (service);
  g_object_notify (G_OBJECT (settings), "service");
  priv->update_service = TRUE;
}

gchar *
empathy_account_settings_get_icon_name (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->icon_name;
}

const gchar *
empathy_account_settings_get_display_name (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->display_name;
}

TpAccount *
empathy_account_settings_get_account (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return priv->account;
}

static gboolean
empathy_account_settings_is_unset (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GArray *a;
  guint i;

  a = priv->unset_parameters;

  for (i = 0; i < a->len; i++)
    {
      if (!tp_strdiff (g_array_index (a, gchar *, i), param))
        return TRUE;
    }

  return FALSE;
}

static const TpConnectionManagerParam *
empathy_account_settings_get_tp_param (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  return tp_protocol_get_param (priv->protocol_obj, param);
}

gboolean
empathy_account_settings_have_tp_param (EmpathyAccountSettings *settings,
    const gchar *param)
{
  return (empathy_account_settings_get_tp_param (settings, param) != NULL);
}

static void
account_settings_remove_from_unset (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  guint idx;
  gchar *val;

  for (idx = 0; idx < priv->unset_parameters->len; idx++)
    {
      val = g_array_index (priv->unset_parameters, gchar *, idx);

      if (!tp_strdiff (val, param))
        {
          priv->unset_parameters =
            g_array_remove_index (priv->unset_parameters, idx);
          g_free (val);

          break;
        }
    }
}

GVariant *
empathy_account_settings_dup_default (EmpathyAccountSettings *settings,
    const gchar *param)
{
  const TpConnectionManagerParam *p;

  p = empathy_account_settings_get_tp_param (settings, param);
  if (p == NULL)
    return NULL;

  return tp_connection_manager_param_dup_default_variant (p);
}

const gchar *
empathy_account_settings_get_dbus_signature (EmpathyAccountSettings *settings,
    const gchar *param)
{
  const TpConnectionManagerParam *p;

  p = empathy_account_settings_get_tp_param (settings, param);

  if (p == NULL)
    return NULL;

  return tp_connection_manager_param_get_dbus_signature (p);
}

static GVariant *
empathy_account_settings_dup (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GVariant *result;

  /* Lookup the update parameters we set */
  result = g_hash_table_lookup (priv->parameters, param);
  if (result != NULL)
    return g_variant_ref (result);

  /* If the parameters isn't unset use the accounts setting if any */
  if (priv->account != NULL
      && !empathy_account_settings_is_unset (settings, param))
    {
      GVariant *parameters;

      parameters = tp_account_dup_parameters_vardict (priv->account);
      result = g_variant_lookup_value (parameters, param, NULL);
      g_variant_unref (parameters);

      if (result != NULL)
        /* g_variant_lookup_value() is (transfer full) */
        return result;
    }

  /* fallback to the default */
  return empathy_account_settings_dup_default (settings, param);
}

void
empathy_account_settings_unset (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  gchar *v;
  if (empathy_account_settings_is_unset (settings, param))
    return;

  if (priv->supports_sasl && !tp_strdiff (param, "password"))
    {
      g_free (priv->password);
      priv->password = NULL;
      return;
    }

  v = g_strdup (param);

  g_array_append_val (priv->unset_parameters, v);
  g_hash_table_remove (priv->parameters, param);
}

void
empathy_account_settings_discard_changes (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  g_hash_table_remove_all (priv->parameters);
  empathy_account_settings_free_unset_parameters (settings);

  g_free (priv->password);
  priv->password = g_strdup (priv->password_original);

  if (priv->account != NULL)
    priv->uri_scheme_tel = empathy_account_has_uri_scheme_tel (priv->account);
  else
    priv->uri_scheme_tel = FALSE;
}

gchar *
empathy_account_settings_dup_string (EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GVariant *v;
  gchar *result = NULL;

  if (!tp_strdiff (param, "password") && priv->supports_sasl)
    {
      return g_strdup (priv->password);
    }

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return NULL;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_STRING))
    result = g_variant_dup_string (v, NULL);

  g_variant_unref (v);
  return result;
}

GStrv
empathy_account_settings_dup_strv (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  GStrv result = NULL;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return NULL;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_STRING_ARRAY))
    result = g_variant_dup_strv (v, NULL);

  g_variant_unref (v);
  return result;
}

gint32
empathy_account_settings_get_int32 (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  gint32 ret = 0;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return 0;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_BYTE))
    ret = g_variant_get_byte (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT32))
    ret = g_variant_get_int32 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT32))
    ret = CLAMP (g_variant_get_uint32 (v), (guint) G_MININT32,
        G_MAXINT32);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT64))
    ret = CLAMP (g_variant_get_int64 (v), G_MININT32, G_MAXINT32);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT64))
    ret = CLAMP (g_variant_get_uint64 (v), (guint64) G_MININT32, G_MAXINT32);
  else
    {
      gchar *tmp;

      tmp = g_variant_print (v, TRUE);
      DEBUG ("Unsupported type for param '%s': %s'", param, tmp);
      g_free (tmp);
    }

  g_variant_unref (v);
  return ret;
}

gint64
empathy_account_settings_get_int64 (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  gint64 ret = 0;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return 0;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_BYTE))
    ret = g_variant_get_byte (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT32))
    ret = g_variant_get_int32 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT32))
    ret = g_variant_get_uint32 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT64))
    ret = g_variant_get_int64 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT64))
    ret = CLAMP (g_variant_get_uint64 (v), (guint64) G_MININT64, G_MAXINT64);
  else
    {
      gchar *tmp;

      tmp = g_variant_print (v, TRUE);
      DEBUG ("Unsupported type for param '%s': %s'", param, tmp);
      g_free (tmp);
    }

  g_variant_unref (v);
  return ret;
}

guint32
empathy_account_settings_get_uint32 (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  guint32 ret = 0;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return 0;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_BYTE))
    ret = g_variant_get_byte (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT32))
    ret = MAX (0, g_variant_get_int32 (v));
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT32))
    ret = g_variant_get_uint32 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT64))
    ret = CLAMP (g_variant_get_int64 (v), 0, G_MAXUINT32);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT64))
    ret = MIN (g_variant_get_uint64 (v), G_MAXUINT32);
  else
    {
      gchar *tmp;

      tmp = g_variant_print (v, TRUE);
      DEBUG ("Unsupported type for param '%s': %s'", param, tmp);
      g_free (tmp);
    }

  g_variant_unref (v);
  return ret;
}

guint64
empathy_account_settings_get_uint64 (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  guint64 ret = 0;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return 0;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_BYTE))
    ret = g_variant_get_byte (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT32))
    ret = MAX (0, g_variant_get_int32 (v));
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT32))
    ret = g_variant_get_uint32 (v);
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_INT64))
    ret = MAX (0, g_variant_get_int64 (v));
  else if (g_variant_is_of_type (v, G_VARIANT_TYPE_UINT64))
    ret = g_variant_get_uint64 (v);
  else
    {
      gchar *tmp;

      tmp = g_variant_print (v, TRUE);
      DEBUG ("Unsupported type for param '%s': %s'", param, tmp);
      g_free (tmp);
    }


  g_variant_unref (v);
  return ret;
}

gboolean
empathy_account_settings_get_boolean (EmpathyAccountSettings *settings,
    const gchar *param)
{
  GVariant *v;
  gboolean result = FALSE;

  v = empathy_account_settings_dup (settings, param);
  if (v == NULL)
    return result;

  if (g_variant_is_of_type (v, G_VARIANT_TYPE_BOOLEAN))
    result = g_variant_get_boolean (v);

  return result;
}

void
empathy_account_settings_set (EmpathyAccountSettings *settings,
    const gchar *param,
    GVariant *v)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  g_return_if_fail (param != NULL);
  g_return_if_fail (v != NULL);

  if (!tp_strdiff (param, "password") && priv->supports_sasl &&
      g_variant_is_of_type (v, G_VARIANT_TYPE_STRING))
    {
      g_free (priv->password);
      priv->password = g_variant_dup_string (v, NULL);
    }
  else
    {
      g_hash_table_insert (priv->parameters, g_strdup (param),
          g_variant_ref_sink (v));
    }

  account_settings_remove_from_unset (settings, param);
}

static void
account_settings_display_name_set_cb (GObject *src,
    GAsyncResult *res,
    gpointer user_data)
{
  GError *error = NULL;
  TpAccount *account = TP_ACCOUNT (src);
  GSimpleAsyncResult *set_result = user_data;

  tp_account_set_display_name_finish (account, res, &error);

  if (error != NULL)
    {
      g_simple_async_result_set_from_error (set_result, error);
      g_error_free (error);
    }

  g_simple_async_result_complete (set_result);
  g_object_unref (set_result);
}

void
empathy_account_settings_set_display_name_async (
  EmpathyAccountSettings *settings,
  const gchar *name,
  GAsyncReadyCallback callback,
  gpointer user_data)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GSimpleAsyncResult *result;

  g_return_if_fail (name != NULL);

  result = g_simple_async_result_new (G_OBJECT (settings),
      callback, user_data, empathy_account_settings_set_display_name_finish);

  if (!tp_strdiff (name, priv->display_name))
    {
      /* Nothing to do */
      g_simple_async_result_complete_in_idle (result);
      return;
    }

  if (priv->account == NULL)
    {
      if (priv->display_name != NULL)
        g_free (priv->display_name);

      priv->display_name = g_strdup (name);

      g_simple_async_result_complete_in_idle (result);

      return;
    }

  tp_account_set_display_name_async (priv->account, name,
      account_settings_display_name_set_cb, result);
}

gboolean
empathy_account_settings_set_display_name_finish (
  EmpathyAccountSettings *settings,
  GAsyncResult *result,
  GError **error)
{
  if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (result),
      error))
    return FALSE;

  g_return_val_if_fail (g_simple_async_result_is_valid (result,
    G_OBJECT (settings), empathy_account_settings_set_display_name_finish),
      FALSE);

  return TRUE;
}

static void
account_settings_icon_name_set_cb (GObject *src,
    GAsyncResult *res,
    gpointer user_data)
{
  GError *error = NULL;
  TpAccount *account = TP_ACCOUNT (src);
  GSimpleAsyncResult *set_result = user_data;

  tp_account_set_icon_name_finish (account, res, &error);

  if (error != NULL)
    {
      g_simple_async_result_set_from_error (set_result, error);
      g_error_free (error);
    }

  g_simple_async_result_complete (set_result);
  g_object_unref (set_result);
}

void
empathy_account_settings_set_icon_name_async (
  EmpathyAccountSettings *settings,
  const gchar *name,
  GAsyncReadyCallback callback,
  gpointer user_data)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GSimpleAsyncResult *result;

  g_return_if_fail (name != NULL);

  result = g_simple_async_result_new (G_OBJECT (settings),
      callback, user_data, empathy_account_settings_set_icon_name_finish);

  if (priv->account == NULL)
    {
      if (priv->icon_name != NULL)
        g_free (priv->icon_name);

      priv->icon_name = g_strdup (name);

      g_simple_async_result_complete_in_idle (result);

      return;
    }

  tp_account_set_icon_name_async (priv->account, name,
      account_settings_icon_name_set_cb, result);
}

gboolean
empathy_account_settings_set_icon_name_finish (
  EmpathyAccountSettings *settings,
  GAsyncResult *result,
  GError **error)
{
  if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (result),
      error))
    return FALSE;

  g_return_val_if_fail (g_simple_async_result_is_valid (result,
    G_OBJECT (settings), empathy_account_settings_set_icon_name_finish),
      FALSE);

  return TRUE;
}

static void
empathy_account_settings_processed_password (GObject *source,
    GAsyncResult *result,
    gpointer user_data,
    gpointer finish_func)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (user_data);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GSimpleAsyncResult *r;
  GError *error = NULL;
  gboolean (*func) (TpAccount *source, GAsyncResult *result, GError **error) =
    finish_func;

  g_free (priv->password_original);
  priv->password_original = g_strdup (priv->password);

  if (!func (TP_ACCOUNT (source), result, &error))
    {
      g_simple_async_result_set_from_error (priv->apply_result, error);
      g_error_free (error);
    }

  empathy_account_settings_discard_changes (settings);

  r = priv->apply_result;
  priv->apply_result = NULL;

  g_simple_async_result_complete (r);
  g_object_unref (r);
}

static void
empathy_account_settings_set_password_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  empathy_account_settings_processed_password (source, result, user_data,
      empathy_keyring_set_account_password_finish);
}

static void
empathy_account_settings_delete_password_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  empathy_account_settings_processed_password (source, result, user_data,
      empathy_keyring_delete_account_password_finish);
}

static void
update_account_uri_schemes (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  if (priv->uri_scheme_tel == empathy_account_has_uri_scheme_tel (
        priv->account))
    return;

  tp_account_set_uri_scheme_association_async (priv->account, "tel",
      priv->uri_scheme_tel, NULL, NULL);
}

static void
set_service_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  GError *error = NULL;

  if (!tp_account_set_service_finish (TP_ACCOUNT (source), result, &error))
    {
      DEBUG ("Failed to set Account.Service: %s", error->message);
      g_error_free (error);
    }
}

static void
update_account_service (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  if (!priv->update_service)
    return;

  tp_account_set_service_async (priv->account,
      priv->service != NULL ? priv->service : "", set_service_cb, self);
}

static void
empathy_account_settings_account_updated (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (user_data);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GSimpleAsyncResult *r;
  GError *error = NULL;
  GStrv reconnect_required = NULL;

  if (!tp_account_update_parameters_vardict_finish (TP_ACCOUNT (source),
          result, &reconnect_required, &error))
    {
      g_simple_async_result_set_from_error (priv->apply_result, error);
      g_error_free (error);
      goto out;
    }

  update_account_uri_schemes (settings);
  update_account_service (settings);

  g_simple_async_result_set_op_res_gboolean (priv->apply_result,
      g_strv_length (reconnect_required) > 0);

  /* Only set the password in the keyring if the CM supports SASL. */
  if (priv->supports_sasl)
    {
      if (priv->password != NULL)
        {
          /* FIXME: we shouldn't save the password if we
           * can't (MaySaveResponse=False) but we don't have API to check that
           * at this point (fdo #35382). */
          empathy_keyring_set_account_password_async (priv->account,
              priv->password, priv->remember_password,
              empathy_account_settings_set_password_cb, settings);
        }
      else
        {
          empathy_keyring_delete_account_password_async (priv->account,
              empathy_account_settings_delete_password_cb, settings);
        }

      return;
    }

out:
  empathy_account_settings_discard_changes (settings);

  r = priv->apply_result;
  priv->apply_result = NULL;

  g_simple_async_result_complete (r);
  g_object_unref (r);
  g_strfreev (reconnect_required);
}

static void
empathy_account_settings_created_cb (GObject *source,
    GAsyncResult *result,
    gpointer user_data)
{
  EmpathyAccountSettings *settings = EMPATHY_ACCOUNT_SETTINGS (user_data);
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GError *error = NULL;
  GSimpleAsyncResult *r;

  priv->account = tp_account_request_create_account_finish (
      TP_ACCOUNT_REQUEST (source), result, &error);

  if (priv->account == NULL)
    {
      g_simple_async_result_set_from_error (priv->apply_result, error);
    }
  else
    {
      if (priv->supports_sasl && priv->password != NULL)
        {
          /* Save the password before connecting */
          /* FIXME: we shouldn't save the password if we
           * can't (MaySaveResponse=False) but we don't have API to check that
           * at this point (fdo #35382). */
          empathy_keyring_set_account_password_async (priv->account,
              priv->password, priv->remember_password,
              empathy_account_settings_set_password_cb,
              settings);
          return;
        }

      update_account_uri_schemes (settings);

      empathy_account_settings_discard_changes (settings);
    }

  r = priv->apply_result;
  priv->apply_result = NULL;

  g_simple_async_result_complete (r);
  g_object_unref (r);
}

static void
empathy_account_settings_do_create_account (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);
  TpAccountRequest *account_req;
  TpConnectionPresenceType type;
  gchar *status;
  gchar *message;
  EmpathyPresenceManager *presence_mgr;
  GHashTableIter iter;
  gpointer k, v;

  account_req = tp_account_request_new (priv->account_manager, priv->cm_name,
      priv->protocol, "New Account");

  presence_mgr = empathy_presence_manager_dup_singleton ();
  type = empathy_presence_manager_get_requested_presence (presence_mgr, &status,
      &message);
  g_object_unref (presence_mgr);

  if (type != TP_CONNECTION_PRESENCE_TYPE_UNSET)
    {
      tp_account_request_set_requested_presence (account_req, type,
          status, message);
    }

  tp_account_request_set_icon_name (account_req, priv->icon_name);

  tp_account_request_set_display_name (account_req, priv->display_name);

  if (priv->service != NULL)
    tp_account_request_set_service (account_req, priv->service);

  g_hash_table_iter_init (&iter, priv->parameters);
  while (g_hash_table_iter_next (&iter, &k, &v))
    {
      const gchar *key = k;
      GVariant *value = v;

      tp_account_request_set_parameter (account_req, key, value);
    }

  if (priv->storage_provider != NULL)
    {
      tp_account_request_set_storage_provider (account_req,
          priv->storage_provider);
    }

  tp_account_request_create_account_async (account_req,
      empathy_account_settings_created_cb, self);
}

static GVariant *
build_parameters_variant (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);
  GVariantBuilder *builder;
  GHashTableIter iter;
  gpointer k, v;

  builder = g_variant_builder_new (G_VARIANT_TYPE_VARDICT);

  g_hash_table_iter_init (&iter, priv->parameters);
  while (g_hash_table_iter_next (&iter, &k, &v))
    {
      const gchar *key = k;
      GVariant *value = v;
      GVariant *entry;

      entry = g_variant_new_dict_entry (g_variant_new_string (key),
          g_variant_new_variant (value));

      g_variant_builder_add_value (builder, entry);
    }

  return g_variant_builder_end (builder);
}

void
empathy_account_settings_apply_async (EmpathyAccountSettings *settings,
    GAsyncReadyCallback callback,
    gpointer user_data)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);

  if (priv->apply_result != NULL)
    {
      g_simple_async_report_error_in_idle (G_OBJECT (settings),
          callback, user_data,
          G_IO_ERROR, G_IO_ERROR_PENDING, "Applying already in progress");
      return;
    }

  priv->apply_result = g_simple_async_result_new (G_OBJECT (settings),
      callback, user_data, empathy_account_settings_apply_finish);

  /* We'll have to reconnect only if we change none DBus_Property on an
   * existing account. */
  g_simple_async_result_set_op_res_gboolean (priv->apply_result, FALSE);

  if (priv->account == NULL)
    {
      g_assert (priv->apply_result != NULL && priv->account == NULL);

      empathy_account_settings_do_create_account (settings);
    }
  else
    {
      tp_account_update_parameters_vardict_async (priv->account,
          build_parameters_variant (settings),
          (const gchar **) priv->unset_parameters->data,
          empathy_account_settings_account_updated, settings);
    }
}

gboolean
empathy_account_settings_apply_finish (EmpathyAccountSettings *settings,
    GAsyncResult *result,
    gboolean *reconnect_required,
    GError **error)
{
  if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (result),
      error))
    return FALSE;

  g_return_val_if_fail (g_simple_async_result_is_valid (result,
    G_OBJECT (settings), empathy_account_settings_apply_finish), FALSE);

  if (reconnect_required != NULL)
    *reconnect_required = g_simple_async_result_get_op_res_gboolean (
        G_SIMPLE_ASYNC_RESULT (result));

  return TRUE;
}

gboolean
empathy_account_settings_has_account (EmpathyAccountSettings *settings,
    TpAccount *account)
{
  EmpathyAccountSettingsPriv *priv;
  const gchar *account_path;
  const gchar *priv_account_path;

  g_return_val_if_fail (EMPATHY_IS_ACCOUNT_SETTINGS (settings), FALSE);
  g_return_val_if_fail (TP_IS_ACCOUNT (account), FALSE);

  priv = GET_PRIV (settings);

  if (priv->account == NULL)
    return FALSE;

  account_path = tp_proxy_get_object_path (TP_PROXY (account));
  priv_account_path = tp_proxy_get_object_path (TP_PROXY (priv->account));

  return (!tp_strdiff (account_path, priv_account_path));
}

void
empathy_account_settings_set_regex (EmpathyAccountSettings *settings,
    const gchar *param,
    const gchar *pattern)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (settings);
  GRegex *regex;
  GError *error = NULL;

  regex = g_regex_new (pattern, 0, 0, &error);
  if (regex == NULL)
    {
      g_warning ("Failed to create reg exp: %s", error->message);
      g_error_free (error);
      return;
    }

  g_hash_table_insert (priv->param_regexps, g_strdup (param), regex);
}

gboolean
empathy_account_settings_parameter_is_valid (
    EmpathyAccountSettings *settings,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv;
  const GRegex *regex;

  g_return_val_if_fail (EMPATHY_IS_ACCOUNT_SETTINGS (settings), FALSE);

  priv = GET_PRIV (settings);

  if (g_list_find_custom (priv->required_params, param, (GCompareFunc) strcmp))
    {
      /* first, look if it's set in our own parameters */
      if (g_hash_table_lookup (priv->parameters, param) != NULL)
        goto test_regex;

      /* if we did not unset the parameter, look if it's in the account */
      if (priv->account != NULL &&
          !empathy_account_settings_is_unset (settings, param))
        {
          const GHashTable *account_params;

          account_params = tp_account_get_parameters (priv->account);
          if (tp_asv_lookup (account_params, param))
            goto test_regex;
        }

      return FALSE;
    }

test_regex:
  /* test whether parameter value matches its regex */
  regex = g_hash_table_lookup (priv->param_regexps, param);
  if (regex)
    {
      gchar *value;
      gboolean match;

      value = empathy_account_settings_dup_string (settings, param);
      if (value == NULL)
        return FALSE;

      match = g_regex_match (regex, value, 0, NULL);

      g_free (value);
      return match;
    }

  return TRUE;
}

gboolean
empathy_account_settings_is_valid (EmpathyAccountSettings *settings)
{
  EmpathyAccountSettingsPriv *priv;
  const gchar *param;
  GHashTableIter iter;
  GList *l;

  g_return_val_if_fail (EMPATHY_IS_ACCOUNT_SETTINGS (settings), FALSE);

  priv = GET_PRIV (settings);

  for (l = priv->required_params; l; l = l->next)
    {
      if (!empathy_account_settings_parameter_is_valid (settings, l->data))
        return FALSE;
    }

  g_hash_table_iter_init (&iter, priv->param_regexps);
  while (g_hash_table_iter_next (&iter, (gpointer *) &param, NULL))
    {
      if (!empathy_account_settings_parameter_is_valid (settings, param))
        return FALSE;
    }

  return TRUE;
}

TpProtocol *
empathy_account_settings_get_tp_protocol (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  return priv->protocol_obj;
}

gboolean
empathy_account_settings_supports_sasl (EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  return priv->supports_sasl;
}

gboolean
empathy_account_settings_param_is_supported (EmpathyAccountSettings *self,
    const gchar *param)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  return tp_protocol_has_param (priv->protocol_obj, param);
}

void
empathy_account_settings_set_uri_scheme_tel (EmpathyAccountSettings *self,
    gboolean associate)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  priv->uri_scheme_tel = associate;
}

gboolean
empathy_account_settings_has_uri_scheme_tel (
    EmpathyAccountSettings *self)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  return priv->uri_scheme_tel;
}

void
empathy_account_settings_set_storage_provider (EmpathyAccountSettings *self,
    const gchar *storage)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  g_free (priv->storage_provider);
  priv->storage_provider = g_strdup (storage);
}

void
empathy_account_settings_set_remember_password (EmpathyAccountSettings *self,
    gboolean remember)
{
  EmpathyAccountSettingsPriv *priv = GET_PRIV (self);

  priv->remember_password = remember;
}
