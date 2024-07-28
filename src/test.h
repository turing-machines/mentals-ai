#pragma once

#include "core.h"


class Test {
public:
    Test() : __member_int(0), __member_double(0.0) {}

    Test(int member_int, double member_double) 
        : __member_int(member_int), __member_double(member_double) {}

    std::unique_ptr<std::string> create_transaction() {
        return std::make_unique<std::string>();
    }

    void set_int(int member_int) {
        __member_int = member_int;
    }

    int get_int() const {
        return __member_int;
    }

    void set_double(double member_double) {
        __member_double = member_double;
    }

    double get_double() const {
        return __member_double;
    }

    void display() const {
        std::cout << "Int: " << __member_int << ", Double: " << __member_double << std::endl;
    }

private:
    int __member_int;
    double __member_double;

    std::shared_ptr<std::string> conn_str;

};
