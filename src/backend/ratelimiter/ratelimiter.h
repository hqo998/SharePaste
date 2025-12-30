#pragma once

class TokenBucket
{
private:
    double capacity {};
    double refillRate {}; //  how often per second to add a token
    double tokens {};
    std::chrono::steady_clock::time_point lastRefillTime;

    void refillTokens();

public:
    TokenBucket(int capacity = 10, double refillRate = 1);
    bool consume(const int amountToConsume);
};

class IpRateLimiter
{
private:
    std::unordered_map<std::string, TokenBucket> ipBuckets;
    int globalCapacity;
    double globalRate;

public:
    IpRateLimiter(int capacity = 10, double refillRate = 1);
    bool allowRequest(std::string_view ipAddress, int consumeAmount);
};