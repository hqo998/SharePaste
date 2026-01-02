#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <shared_mutex>

class TokenBucket
{
private:
    double capacity{};
    double refillRate{}; //  how often per second to add a token
    double tokens{};
    std::chrono::steady_clock::time_point lastRefillTime;

    int blockMaxAttempts;   // how many rate limit failures in a time frame
    int blockAttemptWindow; // how long to check for failed attempts for in mins
    int blockDuration;      // how long to block for in mins
    std::deque<std::chrono::steady_clock::time_point> failureTimestamps;
    std::chrono::steady_clock::time_point blockUntil;

    std::shared_mutex bucketMutex;

    void refillTokens();

public:
    TokenBucket(int capacity = 10, double refillRate = 1, int blockAttemptWindow = 5, int blockDuration = 10, int blockMaxAttempts = 10);
    bool consume(const int amountToConsume);
    bool cleanUpIfOld(int minutes = 60);
    double returnTokensLeft();
    bool isBlocked();
    std::chrono::seconds blockTimeLeft();
};

class IpRateLimiter
{
private:
    std::shared_mutex bucketMapMutex;

    std::unordered_map<std::string, std::shared_ptr<TokenBucket>> ipBuckets;
    int globalCapacity;
    double globalRate;

    int blockMaxAttempts;   // how many rate limit failures in a time frame
    int blockAttemptWindow; // how long to check for failed attempts for in mins
    int blockDuration;      // how long to block for in mins

public:
    IpRateLimiter(int capacity = 10, double refillRate = 1, int blockAttemptWindow = 5, int blockDuration = 10, int blockMaxAttempts = 10);
    bool allowRequest(const std::string &ipAddress, int consumeAmount);
    double checkTokens(const std::string &ipAddress);
    bool isBlocked(const std::string &ipAddress);
    std::chrono::seconds blockTimeLeft(const std::string &ipAddress);
    void cleanAll(int cleanUpMinimumAge);
    int size();
    void printAllIps();
};