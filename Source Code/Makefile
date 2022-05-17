ray_tracer: main.o Sphere.o Ellipsoid.o Triangle.o Standard_light.o Att_light.o Spotlight.o Att_spotlight.o Vectors.o Casting.o Properties.o
	g++ -I. -g -Wall main.o Sphere.o Ellipsoid.o Triangle.o Standard_light.o Att_light.o Spotlight.o Att_spotlight.o Vectors.o Casting.o Properties.o -o ray_tracer -lm

main.o: main.cc Objects.h Lights.h Vectors.h Casting.h Properties.h
	g++ -I. -g -c -Wall main.cc

Sphere.o: Sphere.cc Objects.h
	g++ -I. -g -c -Wall Sphere.cc

Ellipsoid.o: Ellipsoid.cc Objects.h
	g++ -I. -g -c -Wall Ellipsoid.cc

Triangle.o: Triangle.cc Objects.h
	g++ -I. -g -c -Wall Triangle.cc

Standard_light.o: Standard_light.cc Objects.h
	g++ -I. -g -c -Wall Standard_light.cc

Att_light.o: Att_light.cc Lights.h
	g++ -I. -g -c -Wall Att_light.cc

Spotlight.o: Spotlight.cc Lights.h
	g++ -I. -g -c -Wall Spotlight.cc

Att_spotlight.o: Att_spotlight.cc Lights.h
	g++ -I. -g -c -Wall Att_spotlight.cc

Vectors.o: Vectors.h Vectors.cc
	g++ -I. -g -c -Wall Vectors.cc

Casting.o: Casting.h Casting.cc
	g++ -I. -g -c -Wall Casting.cc

Properties.o: Properties.h Properties.cc
	g++ -I. -g -c -Wall Properties.cc

clean:
	rm -f ray_tracer *.o
