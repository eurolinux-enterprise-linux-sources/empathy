/* test-list.c generated by valac 0.20.1, the Vala compiler
 * generated from test-list.vala, do not modify */

/* -*- Mode: vala; indent-tabs-mode: t; c-basic-offset: 2; tab-width: 8 -*- */
/*
 * Copyright (C) 2011 Alexander Larsson <alexl@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <egg-list-box.h>
#include <gdk/gdk.h>

#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_list_free0(var) ((var == NULL) ? NULL : (var = (g_list_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
typedef struct _Block1Data Block1Data;

struct _Block1Data {
	int _ref_count_;
	EggListBox* list;
	GtkLabel* l;
	GtkLabel* l3;
	gint new_button_nr;
};



void update_separator (GtkWidget** separator, GtkWidget* widget, GtkWidget* before);
gint compare_label (GtkWidget* a, GtkWidget* b);
gint compare_label_reverse (GtkWidget* a, GtkWidget* b);
gboolean filter (GtkWidget* widget);
gint _vala_main (gchar** args, int args_length1);
static Block1Data* block1_data_ref (Block1Data* _data1_);
static void block1_data_unref (void * _userdata_);
static void __lambda2_ (GtkWidget* child);
static void ___lambda2__egg_list_box_child_activated (EggListBox* _sender, GtkWidget* child, gpointer self);
static void __lambda3_ (GtkWidget* child);
static void ___lambda3__egg_list_box_child_selected (EggListBox* _sender, GtkWidget* child, gpointer self);
static void __lambda4_ (Block1Data* _data1_);
static gint _compare_label_gcompare_data_func (gconstpointer a, gconstpointer b, gpointer self);
static void ___lambda4__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda5_ (Block1Data* _data1_);
static gint _compare_label_reverse_gcompare_data_func (gconstpointer a, gconstpointer b, gpointer self);
static void ___lambda5__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda6_ (Block1Data* _data1_);
static void ___lambda6__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda7_ (Block1Data* _data1_);
static gboolean _filter_egg_list_box_filter_func (GtkWidget* child, gpointer self);
static void ___lambda7__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda8_ (Block1Data* _data1_);
static void ___lambda8__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda9_ (Block1Data* _data1_);
static void ___lambda9__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda10_ (Block1Data* _data1_);
static void _update_separator_egg_list_box_update_separator_func (GtkWidget** separator, GtkWidget* child, GtkWidget* before, gpointer self);
static void ___lambda10__gtk_button_clicked (GtkButton* _sender, gpointer self);
static void __lambda11_ (Block1Data* _data1_);
static void ___lambda11__gtk_button_clicked (GtkButton* _sender, gpointer self);


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


void update_separator (GtkWidget** separator, GtkWidget* widget, GtkWidget* before) {
	gboolean _tmp0_ = FALSE;
	GtkWidget* _tmp1_;
	gboolean _tmp7_;
	GtkWidget* _tmp35_;
	g_return_if_fail (widget != NULL);
	_tmp1_ = before;
	if (_tmp1_ == NULL) {
		_tmp0_ = TRUE;
	} else {
		gboolean _tmp2_ = FALSE;
		GtkWidget* _tmp3_;
		gboolean _tmp6_;
		_tmp3_ = widget;
		if (G_TYPE_CHECK_INSTANCE_TYPE (_tmp3_, GTK_TYPE_LABEL)) {
			GtkWidget* _tmp4_;
			const gchar* _tmp5_ = NULL;
			_tmp4_ = widget;
			_tmp5_ = gtk_label_get_text (G_TYPE_CHECK_INSTANCE_TYPE (_tmp4_, GTK_TYPE_LABEL) ? ((GtkLabel*) _tmp4_) : NULL);
			_tmp2_ = g_strcmp0 (_tmp5_, "blah3") == 0;
		} else {
			_tmp2_ = FALSE;
		}
		_tmp6_ = _tmp2_;
		_tmp0_ = _tmp6_;
	}
	_tmp7_ = _tmp0_;
	if (_tmp7_) {
		GtkWidget* _tmp8_;
		GtkWidget* _tmp20_;
		GtkBox* _tmp21_;
		GtkBox* hbox;
		GtkWidget* _tmp22_;
		gconstpointer _tmp23_ = NULL;
		gint id;
		GtkBox* _tmp24_;
		GList* _tmp25_ = NULL;
		GList* _tmp26_;
		gconstpointer _tmp27_;
		GtkWidget* _tmp28_;
		GtkLabel* _tmp29_;
		GtkLabel* _tmp30_;
		GtkLabel* l;
		GtkLabel* _tmp31_;
		gint _tmp32_;
		gchar* _tmp33_ = NULL;
		gchar* _tmp34_;
		_tmp8_ = *separator;
		if (_tmp8_ == NULL) {
			GtkBox* _tmp9_;
			GtkBox* hbox;
			GtkLabel* _tmp10_;
			GtkLabel* l;
			GtkBox* _tmp11_;
			GtkLabel* _tmp12_;
			GtkButton* _tmp13_;
			GtkButton* b;
			GtkBox* _tmp14_;
			GtkButton* _tmp15_;
			GtkLabel* _tmp16_;
			GtkButton* _tmp17_;
			GtkBox* _tmp18_;
			GtkWidget* _tmp19_;
			_tmp9_ = (GtkBox*) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
			g_object_ref_sink (_tmp9_);
			hbox = _tmp9_;
			_tmp10_ = (GtkLabel*) gtk_label_new ("Separator");
			g_object_ref_sink (_tmp10_);
			l = _tmp10_;
			_tmp11_ = hbox;
			_tmp12_ = l;
			gtk_container_add ((GtkContainer*) _tmp11_, (GtkWidget*) _tmp12_);
			_tmp13_ = (GtkButton*) gtk_button_new_with_label ("button");
			g_object_ref_sink (_tmp13_);
			b = _tmp13_;
			_tmp14_ = hbox;
			_tmp15_ = b;
			gtk_container_add ((GtkContainer*) _tmp14_, (GtkWidget*) _tmp15_);
			_tmp16_ = l;
			gtk_widget_show ((GtkWidget*) _tmp16_);
			_tmp17_ = b;
			gtk_widget_show ((GtkWidget*) _tmp17_);
			_tmp18_ = hbox;
			_tmp19_ = _g_object_ref0 ((GtkWidget*) _tmp18_);
			_g_object_unref0 (*separator);
			*separator = _tmp19_;
			_g_object_unref0 (b);
			_g_object_unref0 (l);
			_g_object_unref0 (hbox);
		}
		_tmp20_ = *separator;
		_tmp21_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp20_, GTK_TYPE_BOX) ? ((GtkBox*) _tmp20_) : NULL);
		hbox = _tmp21_;
		_tmp22_ = widget;
		_tmp23_ = g_object_get_data ((GObject*) _tmp22_, "sort_id");
		id = (gint) ((gintptr) _tmp23_);
		_tmp24_ = hbox;
		_tmp25_ = gtk_container_get_children ((GtkContainer*) _tmp24_);
		_tmp26_ = _tmp25_;
		_tmp27_ = _tmp26_->data;
		_tmp28_ = (GtkWidget*) _tmp27_;
		_tmp29_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp28_, GTK_TYPE_LABEL) ? ((GtkLabel*) _tmp28_) : NULL);
		_tmp30_ = _tmp29_;
		_g_list_free0 (_tmp26_);
		l = _tmp30_;
		_tmp31_ = l;
		_tmp32_ = id;
		_tmp33_ = g_strdup_printf ("Separator %d", _tmp32_);
		_tmp34_ = _tmp33_;
		gtk_label_set_text (_tmp31_, _tmp34_);
		_g_free0 (_tmp34_);
		_g_object_unref0 (l);
		_g_object_unref0 (hbox);
	} else {
		_g_object_unref0 (*separator);
		*separator = NULL;
	}
	_tmp35_ = *separator;
	g_print ("update separator => %p\n", _tmp35_);
}


gint compare_label (GtkWidget* a, GtkWidget* b) {
	gint result = 0;
	GtkWidget* _tmp0_;
	gconstpointer _tmp1_ = NULL;
	gint aa;
	GtkWidget* _tmp2_;
	gconstpointer _tmp3_ = NULL;
	gint bb;
	g_return_val_if_fail (a != NULL, 0);
	g_return_val_if_fail (b != NULL, 0);
	_tmp0_ = a;
	_tmp1_ = g_object_get_data ((GObject*) _tmp0_, "sort_id");
	aa = (gint) ((gintptr) _tmp1_);
	_tmp2_ = b;
	_tmp3_ = g_object_get_data ((GObject*) _tmp2_, "sort_id");
	bb = (gint) ((gintptr) _tmp3_);
	result = bb - aa;
	return result;
}


gint compare_label_reverse (GtkWidget* a, GtkWidget* b) {
	gint result = 0;
	GtkWidget* _tmp0_;
	GtkWidget* _tmp1_;
	gint _tmp2_ = 0;
	g_return_val_if_fail (a != NULL, 0);
	g_return_val_if_fail (b != NULL, 0);
	_tmp0_ = a;
	_tmp1_ = b;
	_tmp2_ = compare_label (_tmp0_, _tmp1_);
	result = -_tmp2_;
	return result;
}


gboolean filter (GtkWidget* widget) {
	gboolean result = FALSE;
	GtkWidget* _tmp0_;
	const gchar* _tmp1_ = NULL;
	gchar* _tmp2_;
	gchar* text;
	GCompareFunc _tmp3_;
	gint _tmp4_ = 0;
	g_return_val_if_fail (widget != NULL, FALSE);
	_tmp0_ = widget;
	_tmp1_ = gtk_label_get_text (G_TYPE_CHECK_INSTANCE_TYPE (_tmp0_, GTK_TYPE_LABEL) ? ((GtkLabel*) _tmp0_) : NULL);
	_tmp2_ = g_strdup (_tmp1_);
	text = _tmp2_;
	_tmp3_ = g_strcmp0;
	_tmp4_ = _tmp3_ (text, "blah3");
	result = _tmp4_ != 0;
	_g_free0 (text);
	return result;
}


static Block1Data* block1_data_ref (Block1Data* _data1_) {
	g_atomic_int_inc (&_data1_->_ref_count_);
	return _data1_;
}


static void block1_data_unref (void * _userdata_) {
	Block1Data* _data1_;
	_data1_ = (Block1Data*) _userdata_;
	if (g_atomic_int_dec_and_test (&_data1_->_ref_count_)) {
		_g_object_unref0 (_data1_->l3);
		_g_object_unref0 (_data1_->l);
		_g_object_unref0 (_data1_->list);
		g_slice_free (Block1Data, _data1_);
	}
}


static void __lambda2_ (GtkWidget* child) {
	GtkWidget* _tmp0_;
	_tmp0_ = child;
	g_print ("activated %p\n", _tmp0_);
}


static void ___lambda2__egg_list_box_child_activated (EggListBox* _sender, GtkWidget* child, gpointer self) {
	__lambda2_ (child);
}


static void __lambda3_ (GtkWidget* child) {
	GtkWidget* _tmp0_;
	_tmp0_ = child;
	g_print ("selected %p\n", _tmp0_);
}


static void ___lambda3__egg_list_box_child_selected (EggListBox* _sender, GtkWidget* child, gpointer self) {
	__lambda3_ (child);
}


static gint _compare_label_gcompare_data_func (gconstpointer a, gconstpointer b, gpointer self) {
	gint result;
	result = compare_label (a, b);
	return result;
}


static void __lambda4_ (Block1Data* _data1_) {
	egg_list_box_set_sort_func (_data1_->list, _compare_label_gcompare_data_func, NULL, NULL);
}


static void ___lambda4__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda4_ (self);
}


static gint _compare_label_reverse_gcompare_data_func (gconstpointer a, gconstpointer b, gpointer self) {
	gint result;
	result = compare_label_reverse (a, b);
	return result;
}


static void __lambda5_ (Block1Data* _data1_) {
	egg_list_box_set_sort_func (_data1_->list, _compare_label_reverse_gcompare_data_func, NULL, NULL);
}


static void ___lambda5__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda5_ (self);
}


static void __lambda6_ (Block1Data* _data1_) {
	const gchar* _tmp0_ = NULL;
	_tmp0_ = gtk_label_get_text (_data1_->l3);
	if (g_strcmp0 (_tmp0_, "blah3") == 0) {
		gtk_label_set_text (_data1_->l3, "blah5");
		g_object_set_data_full ((GObject*) _data1_->l3, "sort_id", (gpointer) ((gintptr) 5), NULL);
	} else {
		gtk_label_set_text (_data1_->l3, "blah3");
		g_object_set_data_full ((GObject*) _data1_->l3, "sort_id", (gpointer) ((gintptr) 3), NULL);
	}
	egg_list_box_child_changed (_data1_->list, (GtkWidget*) _data1_->l3);
}


static void ___lambda6__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda6_ (self);
}


static gboolean _filter_egg_list_box_filter_func (GtkWidget* child, gpointer self) {
	gboolean result;
	result = filter (child);
	return result;
}


static void __lambda7_ (Block1Data* _data1_) {
	egg_list_box_set_filter_func (_data1_->list, _filter_egg_list_box_filter_func, NULL, NULL);
}


static void ___lambda7__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda7_ (self);
}


static void __lambda8_ (Block1Data* _data1_) {
	egg_list_box_set_filter_func (_data1_->list, NULL, NULL, NULL);
}


static void ___lambda8__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda8_ (self);
}


static void __lambda9_ (Block1Data* _data1_) {
	gchar* _tmp0_ = NULL;
	gchar* _tmp1_;
	GtkLabel* _tmp2_;
	GtkLabel* _tmp3_;
	GtkLabel* ll;
	GtkLabel* _tmp4_;
	gint _tmp5_;
	GtkLabel* _tmp6_;
	_tmp0_ = g_strdup_printf ("blah2 new %d", _data1_->new_button_nr);
	_tmp1_ = _tmp0_;
	_tmp2_ = (GtkLabel*) gtk_label_new (_tmp1_);
	g_object_ref_sink (_tmp2_);
	_tmp3_ = _tmp2_;
	_g_free0 (_tmp1_);
	ll = _tmp3_;
	_tmp4_ = _data1_->l;
	g_object_set_data_full ((GObject*) _tmp4_, "sort_id", (gpointer) ((gintptr) _data1_->new_button_nr), NULL);
	_tmp5_ = _data1_->new_button_nr;
	_data1_->new_button_nr = _tmp5_ + 1;
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) ll);
	_tmp6_ = _data1_->l;
	gtk_widget_show ((GtkWidget*) _tmp6_);
	_g_object_unref0 (ll);
}


static void ___lambda9__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda9_ (self);
}


static void _update_separator_egg_list_box_update_separator_func (GtkWidget** separator, GtkWidget* child, GtkWidget* before, gpointer self) {
	update_separator (separator, child, before);
}


static void __lambda10_ (Block1Data* _data1_) {
	egg_list_box_set_separator_funcs (_data1_->list, _update_separator_egg_list_box_update_separator_func, NULL, NULL);
}


static void ___lambda10__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda10_ (self);
}


static void __lambda11_ (Block1Data* _data1_) {
	egg_list_box_set_separator_funcs (_data1_->list, NULL, NULL, NULL);
}


static void ___lambda11__gtk_button_clicked (GtkButton* _sender, gpointer self) {
	__lambda11_ (self);
}


gint _vala_main (gchar** args, int args_length1) {
	gint result = 0;
	Block1Data* _data1_;
	GtkWindow* _tmp0_;
	GtkWindow* w;
	GtkBox* _tmp1_;
	GtkBox* hbox;
	EggListBox* _tmp2_;
	GtkLabel* _tmp3_;
	GtkLabel* _tmp4_;
	GtkLabel* _tmp5_;
	GtkLabel* _tmp6_;
	GtkLabel* _tmp7_;
	GtkLabel* _tmp8_;
	GtkLabel* _tmp9_;
	GtkLabel* _tmp10_;
	GtkLabel* _tmp11_;
	GtkLabel* _tmp12_;
	GtkBox* _tmp13_;
	GtkBox* row_vbox;
	GtkBox* _tmp14_;
	GtkBox* row_hbox;
	GtkLabel* _tmp15_;
	GtkLabel* _tmp16_;
	GtkCheckButton* _tmp17_;
	GtkCheckButton* check;
	GtkCheckButton* _tmp18_;
	GtkButton* _tmp19_;
	GtkButton* button;
	GtkButton* _tmp20_;
	GtkCheckButton* _tmp21_;
	GtkCheckButton* _tmp22_;
	GtkButton* _tmp23_;
	GtkButton* _tmp24_;
	GtkButton* _tmp25_;
	GtkButton* _tmp26_;
	GtkBox* _tmp27_;
	GtkBox* vbox;
	GtkButton* _tmp28_;
	GtkButton* b;
	GtkButton* _tmp29_;
	GtkButton* _tmp30_;
	GtkButton* _tmp31_;
	GtkButton* _tmp32_;
	GtkButton* _tmp33_;
	GtkButton* _tmp34_;
	GtkButton* _tmp35_;
	GtkButton* _tmp36_;
	GtkButton* _tmp37_;
	GtkButton* _tmp38_;
	GtkButton* _tmp39_;
	GtkButton* _tmp40_;
	GtkButton* _tmp41_;
	GtkButton* _tmp42_;
	GtkButton* _tmp43_;
	GtkButton* _tmp44_;
	GtkButton* _tmp45_;
	GtkButton* _tmp46_;
	GtkButton* _tmp47_;
	GtkButton* _tmp48_;
	GtkButton* _tmp49_;
	GtkButton* _tmp50_;
	GtkButton* _tmp51_;
	GtkCssProvider* _tmp52_;
	GtkCssProvider* provider;
	GdkScreen* _tmp53_ = NULL;
	GError * _inner_error_ = NULL;
	_data1_ = g_slice_new0 (Block1Data);
	_data1_->_ref_count_ = 1;
	gtk_init (&args_length1, &args);
	_tmp0_ = (GtkWindow*) gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_object_ref_sink (_tmp0_);
	w = _tmp0_;
	_tmp1_ = (GtkBox*) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	g_object_ref_sink (_tmp1_);
	hbox = _tmp1_;
	gtk_container_add ((GtkContainer*) w, (GtkWidget*) hbox);
	_tmp2_ = egg_list_box_new ();
	g_object_ref_sink (_tmp2_);
	_data1_->list = _tmp2_;
	gtk_container_add ((GtkContainer*) hbox, (GtkWidget*) _data1_->list);
	g_signal_connect (_data1_->list, "child-activated", (GCallback) ___lambda2__egg_list_box_child_activated, NULL);
	g_signal_connect (_data1_->list, "child-selected", (GCallback) ___lambda3__egg_list_box_child_selected, NULL);
	_tmp3_ = (GtkLabel*) gtk_label_new ("blah4");
	g_object_ref_sink (_tmp3_);
	_data1_->l = _tmp3_;
	_tmp4_ = _data1_->l;
	g_object_set_data_full ((GObject*) _tmp4_, "sort_id", (gpointer) ((gintptr) 4), NULL);
	_tmp5_ = _data1_->l;
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) _tmp5_);
	_tmp6_ = (GtkLabel*) gtk_label_new ("blah3");
	g_object_ref_sink (_tmp6_);
	_data1_->l3 = _tmp6_;
	g_object_set_data_full ((GObject*) _data1_->l3, "sort_id", (gpointer) ((gintptr) 3), NULL);
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) _data1_->l3);
	_tmp7_ = (GtkLabel*) gtk_label_new ("blah1");
	g_object_ref_sink (_tmp7_);
	_g_object_unref0 (_data1_->l);
	_data1_->l = _tmp7_;
	_tmp8_ = _data1_->l;
	g_object_set_data_full ((GObject*) _tmp8_, "sort_id", (gpointer) ((gintptr) 1), NULL);
	_tmp9_ = _data1_->l;
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) _tmp9_);
	_tmp10_ = (GtkLabel*) gtk_label_new ("blah2");
	g_object_ref_sink (_tmp10_);
	_g_object_unref0 (_data1_->l);
	_data1_->l = _tmp10_;
	_tmp11_ = _data1_->l;
	g_object_set_data_full ((GObject*) _tmp11_, "sort_id", (gpointer) ((gintptr) 2), NULL);
	_tmp12_ = _data1_->l;
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) _tmp12_);
	_tmp13_ = (GtkBox*) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	g_object_ref_sink (_tmp13_);
	row_vbox = _tmp13_;
	_tmp14_ = (GtkBox*) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	g_object_ref_sink (_tmp14_);
	row_hbox = _tmp14_;
	g_object_set_data_full ((GObject*) row_vbox, "sort_id", (gpointer) ((gintptr) 3), NULL);
	_tmp15_ = (GtkLabel*) gtk_label_new ("da box for da man");
	g_object_ref_sink (_tmp15_);
	_g_object_unref0 (_data1_->l);
	_data1_->l = _tmp15_;
	_tmp16_ = _data1_->l;
	gtk_container_add ((GtkContainer*) row_hbox, (GtkWidget*) _tmp16_);
	_tmp17_ = (GtkCheckButton*) gtk_check_button_new ();
	g_object_ref_sink (_tmp17_);
	check = _tmp17_;
	_tmp18_ = check;
	gtk_container_add ((GtkContainer*) row_hbox, (GtkWidget*) _tmp18_);
	_tmp19_ = (GtkButton*) gtk_button_new_with_label ("ya!");
	g_object_ref_sink (_tmp19_);
	button = _tmp19_;
	_tmp20_ = button;
	gtk_container_add ((GtkContainer*) row_hbox, (GtkWidget*) _tmp20_);
	gtk_container_add ((GtkContainer*) row_vbox, (GtkWidget*) row_hbox);
	_tmp21_ = (GtkCheckButton*) gtk_check_button_new ();
	g_object_ref_sink (_tmp21_);
	_g_object_unref0 (check);
	check = _tmp21_;
	_tmp22_ = check;
	gtk_container_add ((GtkContainer*) row_vbox, (GtkWidget*) _tmp22_);
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) row_vbox);
	_tmp23_ = (GtkButton*) gtk_button_new_with_label ("focusable row");
	g_object_ref_sink (_tmp23_);
	_g_object_unref0 (button);
	button = _tmp23_;
	_tmp24_ = button;
	gtk_widget_set_hexpand ((GtkWidget*) _tmp24_, FALSE);
	_tmp25_ = button;
	gtk_widget_set_halign ((GtkWidget*) _tmp25_, GTK_ALIGN_START);
	_tmp26_ = button;
	gtk_container_add ((GtkContainer*) _data1_->list, (GtkWidget*) _tmp26_);
	_tmp27_ = (GtkBox*) gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	g_object_ref_sink (_tmp27_);
	vbox = _tmp27_;
	gtk_container_add ((GtkContainer*) hbox, (GtkWidget*) vbox);
	_tmp28_ = (GtkButton*) gtk_button_new_with_label ("sort");
	g_object_ref_sink (_tmp28_);
	b = _tmp28_;
	_tmp29_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp29_);
	_tmp30_ = b;
	g_signal_connect_data (_tmp30_, "clicked", (GCallback) ___lambda4__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp31_ = (GtkButton*) gtk_button_new_with_label ("reverse");
	g_object_ref_sink (_tmp31_);
	_g_object_unref0 (b);
	b = _tmp31_;
	_tmp32_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp32_);
	_tmp33_ = b;
	g_signal_connect_data (_tmp33_, "clicked", (GCallback) ___lambda5__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp34_ = (GtkButton*) gtk_button_new_with_label ("change");
	g_object_ref_sink (_tmp34_);
	_g_object_unref0 (b);
	b = _tmp34_;
	_tmp35_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp35_);
	_tmp36_ = b;
	g_signal_connect_data (_tmp36_, "clicked", (GCallback) ___lambda6__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp37_ = (GtkButton*) gtk_button_new_with_label ("filter");
	g_object_ref_sink (_tmp37_);
	_g_object_unref0 (b);
	b = _tmp37_;
	_tmp38_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp38_);
	_tmp39_ = b;
	g_signal_connect_data (_tmp39_, "clicked", (GCallback) ___lambda7__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp40_ = (GtkButton*) gtk_button_new_with_label ("unfilter");
	g_object_ref_sink (_tmp40_);
	_g_object_unref0 (b);
	b = _tmp40_;
	_tmp41_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp41_);
	_tmp42_ = b;
	g_signal_connect_data (_tmp42_, "clicked", (GCallback) ___lambda8__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_data1_->new_button_nr = 1;
	_tmp43_ = (GtkButton*) gtk_button_new_with_label ("add");
	g_object_ref_sink (_tmp43_);
	_g_object_unref0 (b);
	b = _tmp43_;
	_tmp44_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp44_);
	_tmp45_ = b;
	g_signal_connect_data (_tmp45_, "clicked", (GCallback) ___lambda9__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp46_ = (GtkButton*) gtk_button_new_with_label ("separate");
	g_object_ref_sink (_tmp46_);
	_g_object_unref0 (b);
	b = _tmp46_;
	_tmp47_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp47_);
	_tmp48_ = b;
	g_signal_connect_data (_tmp48_, "clicked", (GCallback) ___lambda10__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	_tmp49_ = (GtkButton*) gtk_button_new_with_label ("unseparate");
	g_object_ref_sink (_tmp49_);
	_g_object_unref0 (b);
	b = _tmp49_;
	_tmp50_ = b;
	gtk_container_add ((GtkContainer*) vbox, (GtkWidget*) _tmp50_);
	_tmp51_ = b;
	g_signal_connect_data (_tmp51_, "clicked", (GCallback) ___lambda11__gtk_button_clicked, block1_data_ref (_data1_), (GClosureNotify) block1_data_unref, 0);
	gtk_widget_show_all ((GtkWidget*) w);
	_tmp52_ = gtk_css_provider_new ();
	provider = _tmp52_;
	gtk_css_provider_load_from_data (provider, "\n" \
"EggListBox:prelight {\n" \
"background-color: green;\n" \
"}\n" \
"EggListBox:active {\n" \
"background-color: red;\n" \
"}\n", (gssize) (-1), &_inner_error_);
	if (_inner_error_ != NULL) {
		_g_object_unref0 (provider);
		_g_object_unref0 (b);
		_g_object_unref0 (vbox);
		_g_object_unref0 (button);
		_g_object_unref0 (check);
		_g_object_unref0 (row_hbox);
		_g_object_unref0 (row_vbox);
		_g_object_unref0 (hbox);
		_g_object_unref0 (w);
		block1_data_unref (_data1_);
		_data1_ = NULL;
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return 0;
	}
	_tmp53_ = gdk_screen_get_default ();
	gtk_style_context_add_provider_for_screen (_tmp53_, (GtkStyleProvider*) provider, (guint) 600);
	gtk_main ();
	result = 0;
	_g_object_unref0 (provider);
	_g_object_unref0 (b);
	_g_object_unref0 (vbox);
	_g_object_unref0 (button);
	_g_object_unref0 (check);
	_g_object_unref0 (row_hbox);
	_g_object_unref0 (row_vbox);
	_g_object_unref0 (hbox);
	_g_object_unref0 (w);
	block1_data_unref (_data1_);
	_data1_ = NULL;
	return result;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return _vala_main (argv, argc);
}



