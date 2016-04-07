#pragma once
#define SERIALIZER public: void Relocate(long, void *);
#define PTR(X, x)	union { X x; int64_t _x; };
#define _TAG
