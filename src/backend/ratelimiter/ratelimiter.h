#pragma once

class TokenBucket
{
private:
    double capacity {};
    double refillRate {}; //  how often per second to add a token
    double tokens {};

    std::chrono::steady_clock::time_point lastRefillTime;

    TokenBucket(int capacity = 10, double refillRate = 1);

    void refillTokens();

public:
    bool consume(const int amountToConsume);
};