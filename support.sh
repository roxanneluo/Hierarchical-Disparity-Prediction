#!/bin/bash
pic=Baby1
echo "~~~~~~"$pic"~~~~~~~~~"
points="432 422 455 476 478 381 391 491 156 401 132 474"
echo "--UST--"
alg=Support_UST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points
echo "--MST--"
alg=Support_MST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points


pic=Flowerpots
echo "~~~~~~"$pic"~~~~~~~~~"
points="215 241 324 209 352 237 356 256 194 291"
echo "--MST--"
alg=Support_MST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points
echo "--UST--"
alg=Support_UST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points

pic=Wood1
echo "~~~~~~"$pic"~~~~~~~~~"
points="516 112 465 395 527 293"
echo "--MST--"
alg=Support_MST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points
echo "--UST--"
alg=Support_UST
./bin/$alg.out testdata/halfsize/$pic/left.ppm testdata/halfsize/$pic/right.ppm 100 3 haha haha bin/stable/$pic"_left_support_map" $alg $points


