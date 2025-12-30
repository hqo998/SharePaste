#pragma once

#include <deque>

class TokenBucket
{
private:
    double capacity {};
    double refillRate {}; //  how often per second to add a token
    double tokens {};
    std::chrono::steady_clock::time_point lastRefillTime;

    int maxAttempts {5}; // how many rate limit failures in a time frame
    int blockAttemptWindow {1}; // how long to check for failed attempts for in mins
    int blockDuration {10}; // how long to block for in mins
    std::deque<std::chrono::steady_clock::time_point> failureTimestamps;
    std::chrono::steady_clock::time_point blockUntil;

    void refillTokens();

public:
    TokenBucket(int capacity = 10, double refillRate = 1);
    bool consume(const int amountToConsume);
    bool cleanUpIfOld(int minutes = 60);
    double returnTokensLeft();
};

class IpRateLimiter
{
private:
    std::unordered_map<std::string, TokenBucket> ipBuckets;
    int globalCapacity;
    double globalRate;
    int cleanUpInteval = 30;

public:
    IpRateLimiter(int capacity = 10, double refillRate = 1);
    bool allowRequest(const std::string& ipAddress, int consumeAmount);
    double checkTokens(const std::string& ipAddress);

    void cleanAll();
    void printAllIps();

};