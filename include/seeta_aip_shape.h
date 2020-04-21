//
// Created by SeetaTech on 2020/4/20.
//

#ifndef _INC_SEETA_AIP_SHAPE_H
#define _INC_SEETA_AIP_SHAPE_H

#include "seeta_aip_struct.h"

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

            std::string str() const override {
                std::ostringstream oss;

                oss << "Points";
                PlotPoints(oss, this->landmarks());

                return oss.str();
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

            std::string str() const override {
                std::ostringstream oss;

                oss << "Lines";
                PlotPoints(oss, this->landmarks());

                return oss.str();
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

            std::string str() const override {
                std::ostringstream oss;

                auto &left_top = this->landmarks()[0];
                auto &right_bottom = this->landmarks()[1];

                oss << "Rectangle{";
                oss << "x=" << left_top.x
                    << ", y=" << left_top.y
                    << ", width=" << right_bottom.x - left_top.x
                    << ", height=" << right_bottom.y - left_top.y;
                oss << "}";

                return oss.str();
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

            std::string str() const override {
                std::ostringstream oss;

                auto &first = this->landmarks()[0];
                auto &second = this->landmarks()[1];
                auto &third = this->landmarks()[2];

                oss << "Parallelogram{";
                oss << "first=" << first
                    << ", second=" << second
                    << ", third=" << third;
                oss << "}";

                return oss.str();
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

            std::string str() const override {
                std::ostringstream oss;

                oss << "Polygon";
                PlotPoints(oss, this->landmarks());

                return oss.str();
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

            const Point &center() const {
                return landmarks()[0];
            }

            float radius() const {
                return scale();
            }

            std::string str() const override {
                std::ostringstream oss;

                oss << "Circle{";
                oss << "center=" << center()
                    << ", radius=" << radius();
                oss << "}";

                return oss.str();
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

            std::string str() const override {
                std::ostringstream oss;

                auto &first = this->landmarks()[0];
                auto &second = this->landmarks()[1];
                auto &third = this->landmarks()[2];

                oss << "Cube{";
                oss << "left_top_front=" << first
                    << ", right_bottom_front=" << second
                    << ", right_top_back=" << third;
                oss << "}";

                return oss.str();
            }
        };
    }
}

#endif //_INC_SEETA_AIP_SHAPE_H
