# specific for catapult.xrc

/name="CatapultFrame"/{
	s/class="CatapultFrame"/class="wxFrame"/
	s/>/ subclass="CatapultFrame">/
}
/<tabs>/,/<\/object>/d
s/\([^<]*\)\(<object.*" \)\(name="GlobalTabControl" .*>\)/\1\2\3\
\1    <usenotebooksizer>1<\/usenotebooksizer>/

/name="SessionPage"/,${
	/selection/d
	/<choices>/,/<\/choices>/d
}

/name="VideoControlPage"/,${
	s/choices/content/
	s/choice/item/g
}

#global deletes

/<option>0<\/option>/d
/<style>0<\/style>/d
/<border>0<\/border>/d
/Sizer/s/ name="[^"]*"//
s/ base="[^"]*"//
/<object class="wxStatic/,/<\/object>/{
	/attribute/d
}

# global replaces

s/\.\.\/resources\/bitmaps/\.\.\/bitmaps/
s/<application path.*>/<resource version="2.3.0.1">/
s/<disabled>1<\/disabled>/<enabled>0<\/enabled>/
s/<\/application>/<\/resource>/
s/\([^<]*\)<range>\([0-9]*\), *\([0-9]*\)<\/range>/\1<min>\2<\/min>\
\1<max>\3<\/max>/

