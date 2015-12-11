
efl = require('efl');
elm = require('elm');

win = new elm.Elm.WinStandard(null);
win.setTitle("Icon Plain");
win.setAutohide(true);

icon = new elm.Elm.Icon(win);
//icon.order_lookup_set(ELM_ICON_LOOKUP_THEME_FDO); Ja eh o default
icon.setStandard("home");

var path, group;
r = icon.getFile(path, group);
path = r[0];
gruop = r[1];
console.log("path = " + path + ", group = " + group);

var name;
name = icon.getStandard();
console.log("name = " + name);

icon.setNoScale(true);
icon.setResizable(false, true);
icon.setSmooth(false);
icon.setFillOutside(true);

icon.setSizeHintWeight(1.0, 1.0);
win.resizeObjectAdd(icon);
icon.setVisible(true);

win.setSize(320, 320);
win.setVisible(true);
