//
// Created by SeetaTech on 2020/4/20.
//

#ifndef _INC_SEETA_AIP_SHAPE_H
#define _INC_SEETA_AIP_SHAPE_H

#include "seeta_aip_cpp.h"

namespace seeta {
    namespace aip {
        class Points : public Shape {
        public:
            using self = Points;

            Points() {
                type(SEETA_AIP_POINTS);
            }

            explicit Points(const std::vector<Point> &points)
                    : self() {
                landmarks(points);
            }
        };

        class Lines : public Shape {
        public:
            using self = Lines;

            Lines() {
                type(SEETA_AIP_LINES);
            }

            explicit Lines(const std::vector<Point> &points)
                    : self() {
                landmarks(points);
            }
        };

        class Rectangle : public Shape {
        public:
            using self = Rectangle;

            Rectangle() {
                type(SEETA_AIP_RECTANGLE);
            }

            explicit Rectangle(const Point &left_top, const Point &right_bottom, float rotate = 0)
                    : self() {
                this->landmarks({left_top, right_bottom});
                this->rotate(rotate);
            }

            explicit Rectangle(float x, float y, float width, float height, float rotate = 0)
                    : self() {
                this->landmarks({{x,         y},
                                 {x + width, y + height}});
                this->rotate(rotate);
            }
        };

        class Parallelogram : public Shape {
        public:
            using self = Parallelogram;

            Parallelogram() {
                type(SEETA_AIP_PARALLELOGRAM);
            }


            explicit Parallelogram(const Point &first, const Point &second, const Point &third)
                    : self() {
                this->landmarks({first, second, third});
            }

        };

        class Polygon : public Shape {
        public:
            using self = Polygon;

            Polygon() {
                type(SEETA_AIP_POLYGON);
            }

            explicit Polygon(const std::vector<Point> &points)
                    : self() {
                landmarks(points);
            }
        };

        class Circle : public Shape {
        public:
            using self = Circle;

            Circle() {
                type(SEETA_AIP_CIRCLE);
            }

            explicit Circle(const Point &center, float radius)
                    : self() {
                landmarks({center});
                scale(radius);
            }
        };

        class Cube : public Shape {
        public:
            using self = Cube;

            Cube() {
                type(SEETA_AIP_CUBE);
            }

            explicit Cube(const Point &left_top_front, const Point &right_bottom_front, const Point &right_top_back)
                    : self() {
                landmarks({left_top_front, right_bottom_front, right_top_back});
            }
        };
    }
}

#endif //_INC_SEETA_AIP_SHAPE_H
