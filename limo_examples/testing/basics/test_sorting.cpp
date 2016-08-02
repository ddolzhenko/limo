#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "limo/test_main.hpp"

template <class T>
std::ostream& operator<<(std::ostream& o, const std::vector<T>& v) {
    o << "[";
    for(const auto& x : v)
        o << x << ", ";
    return o << "]";
}

template <class TIter>
void selection_sort(TIter begin, TIter end) {
    // [begin, pivot) [pivot, end)
    // [sorted) [unsorted)
    for(TIter pivot = begin; pivot < end; ++pivot) {
        std::iter_swap(pivot, std::min_element(pivot, end));
    }
}

template <class TFunc, class TParam>
decltype(auto) out_param(TFunc func, TParam param) {
    func(param);
    return param;
}

LTEST(sorting) {
    using namespace std;
    using namespace limo;

    typedef std::vector<int>::iterator IT;
    typedef std::vector<int> Vec;


    auto adaptor = [](std::function<void(IT, IT)> algo) {
        return [algo](const vector<int>& data) {
            vector<int> result = data;
            algo(result.begin(), result.end());
            return result;
        };
    };


    #define LCONTEXT(...)
    
    for(auto sort : {adaptor(selection_sort<IT>), adaptor(std::sort<IT>)} )
    {

        // LSUBTEST(degenerated);
        // EXPECT({} == sort({}));

      
        // LSUBTEST(trivial);
        LTEST(trivial, sort) {
            EXPECT_TRUE(Vec({1, 2, 42}) == sort({2, 42, 1}));
        };
    }
            
      
};
