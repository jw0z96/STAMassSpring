#ifndef SPRING_H_
#define SPRING_H_

struct Spring
{
	Spring(int _s, int _e): startPoint(_s), endPoint(_e) {}
	int startPoint;
	int endPoint;
};

#endif // SPRING_H_
