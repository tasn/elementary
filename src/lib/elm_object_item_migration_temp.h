// Item Eo migration temp defs - to be removed
typedef Eo Elm_Widobj_Item;

#undef VIEW
#undef WIDGET
#define VIEW(X)   X->base->view
#define WIDGET(X) X->base->widget