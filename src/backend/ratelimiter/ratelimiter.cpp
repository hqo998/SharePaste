// rate limiter based off 'Token Bucket algorithm' as seen here https://smudge.ai/blog/ratelimit-algorithms
// https://medium.com/@sahilbitsp/rate-limiting-algorithms-c-d185f942a7db

//https://dev.to/0xtanzim/token-bucket-algorithm-explained-4ceo
// C = bucket capacity
// R = refill rate (tokens per second)
// T = elapsed time since last refill
// then the number of tokens at any moment is:
// tokens = min(C, tokens + R * T)
// when a request comes in:
// if tokens > 0 → allow and tokens -= 1
// else → reject

// https://github.com/rigtorp/TokenBucket/blob/master/TokenBucket.h


// design considerations
// Needs individual token buckets for every ip address
// Only calculate how many tokens a bucket should be refilled when a request that uses said bucket comes in, instead of a background process constantly updating


// how i want to access the bucket
/*
bool tokenBucket.consume(ipAddress);
*/



#include <chrono>
#include <thread>
#include <string>

#include <utility.h>
#include <ratelimiter.h>

class IpRateLimiter
{
private:
    std::unordered_map<std::string, TokenBucket> ipBuckets;

    double globalCapacity;
    double globalGlobalRate;

    IpRateLimiter(int capacity = 10, double refillRate = 1)
{
    this->globalCapacity = capacity;
    this->globalGlobalRate = refillRate;
}

public:
    bool allowRequest(std::string_view ipAddress)
    {
        
    }
};


TokenBucket::TokenBucket(int capacity, double refillRate)
{
    this->capacity = capacity;
    this->refillRate = refillRate;
    this->tokens = capacity;
    this->lastRefillTime = std::chrono::steady_clock::now();
}

void TokenBucket::refillTokens()
{
    auto rightNow = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(rightNow - lastRefillTime);
    if (elapsedTime.count() > 0)
    {
        double newTokens = static_cast<double>((elapsedTime.count() / 1000) * refillRate);
        this->tokens = std::min(capacity, this->tokens + newTokens); // never goes higher then capacity
        lastRefillTime = std::chrono::steady_clock::now();
    }
}

bool TokenBucket::consume(const int amountToConsume)
{
    refillTokens();
    if (tokens >= amountToConsume)
    {
        tokens =- amountToConsume;
        return true;
    }
    else
    {
        return false;
    }
}

