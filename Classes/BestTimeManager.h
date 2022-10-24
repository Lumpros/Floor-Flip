//
// Created by Lumpros on 10/20/2022.
//

#ifndef __BEST_TIME_MANAGER_H__
#define __BEST_TIME_MANAGER_H__

#include <string>

class BestTimeManager
{
public:
    BestTimeManager(const BestTimeManager&) = delete;
    BestTimeManager& operator=(const BestTimeManager&) = delete;

    static BestTimeManager* getInstance();

    std::string getBestTime(const std::string& mode);

    // Returns true if the record was saved (and therefore if it is the best time thus far)
    // Returns false otherwise
    bool saveBestTime(const std::string& mode, const std::string& time);

private:
    BestTimeManager() = default;
};


#endif