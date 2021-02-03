#pragma once

#include "nocopyable.h"

#include <map>
#include <atomic>

template <typename NotifyFunc>
class OBEvent:  Nocopyable {


    public:

        OBEvent(){
            ob_num = 0;
        };


        int Register(NotifyFunc&& f) {
            return AssignObserver(f);
        }




        int UnRegister(int key) {
            container_.erase(key);
            return 0;
        }


        template<typename... Args>
        int NotifyAll(Args&&... args) {
            int cnt = 0;
            for (auto& item : container_) {

                item.second(std::forward<Args>(args)...);

                cnt ++;
            }
            return cnt;
        }


        template<typename F>
        int AssignObserver(F&& f) {
            int idx = ob_num ++;
            container_.emplace(idx, std::forward<F>(f));
            return idx;
        }


    private:

    std::atomic<int> ob_num;

    std::map<int, NotifyFunc> container_; 
};




