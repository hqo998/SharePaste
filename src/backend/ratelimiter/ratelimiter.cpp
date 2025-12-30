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
#include <string>
#include <algorithm>
#include <deque>

#include <utility.h>
#include <ratelimiter.h>
#include <wincrypt.h>


IpRateLimiter::IpRateLimiter(int capacity, double refillRate, int blockAttemptWindow, int blockDuration, int blockMaxAttempts)
{
    this->globalCapacity = capacity;
    this->globalRate = refillRate;
    this->blockAttemptWindow = blockAttemptWindow;
    this->blockDuration = blockDuration;
    this->blockMaxAttempts = blockMaxAttempts;
}

bool IpRateLimiter::allowRequest(const std::string& ipAddress, int consumeAmount)
{
    if (globalCapacity == 0) return true; // If capacity is disabled disable checking.

    if (!ipBuckets.contains(ipAddress))
    {
        ipBuckets.emplace(ipAddress, TokenBucket(globalCapacity, globalRate, blockAttemptWindow, blockDuration, blockMaxAttempts));
    }

    return ipBuckets.at(ipAddress).consume(consumeAmount);
}

void IpRateLimiter::cleanAll()
{

    std::erase_if(ipBuckets, [this](auto& bucketMap) {
    auto& [ip, bucket] = bucketMap;
    bool shouldRemove = bucket.cleanUpIfOld(cleanUpInteval); // clean up minutes

    // Debug print
    std::cout << "Checking IP: " << ip << " | Should remove: " << std::boolalpha << shouldRemove << std::endl;

    return shouldRemove;
});
}

void IpRateLimiter::printAllIps()
{
    sharepaste::printLine("Available IP, printing that it works to call.");
    for (auto& [ip, tokenBucket] : ipBuckets)
    {
        sharepaste::printLine("Available IP in memory: {}", ip);
    }
}

double IpRateLimiter::checkTokens(const std::string& ipAddress)
{
    if (!ipBuckets.contains(ipAddress))
    {
        return globalCapacity;
    }
    return ipBuckets.at(ipAddress).returnTokensLeft();
}

bool IpRateLimiter::isBlocked(const std::string& ipAddress)
{
    if (!ipBuckets.contains(ipAddress))
    {
        return false;
    }
    return ipBuckets.at(ipAddress).isBlocked();
}

std::chrono::seconds IpRateLimiter::blockTimeLeft(const std::string& ipAddress)
{
    if (!ipBuckets.contains(ipAddress))
    {
        return std::chrono::seconds(0);
    }
    return ipBuckets.at(ipAddress).blockTimeLeft();
}
// needs a way to call isblocked and block time left in for each ip address


TokenBucket::TokenBucket(int capacity, double refillRate, int blockAttemptWindow, int blockDuration, int blockMaxAttempts)
{
    this->capacity = capacity;
    this->refillRate = refillRate;
    this->tokens = capacity;
    this->lastRefillTime = std::chrono::steady_clock::now();
    this->blockAttemptWindow = blockAttemptWindow;
    this->blockDuration = blockDuration;
    this->blockMaxAttempts = blockMaxAttempts;
}

void TokenBucket::refillTokens()
{
    auto rightNow = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(rightNow - lastRefillTime);
    if (elapsedTime.count() > 0)
    {
        double newTokens = static_cast<double>((static_cast<double>(elapsedTime.count()) / 1000) * refillRate);
        this->tokens = std::min(capacity, this->tokens + newTokens); // never goes higher then capacity
        lastRefillTime = std::chrono::steady_clock::now();
    }
}

bool TokenBucket::consume(const int amountToConsume)
{
    auto rightNow = std::chrono::steady_clock::now();

    if (capacity == 0) return true; // to disable rate limiting set capacity to 0

    if (rightNow < blockUntil) return false; // long blocks for repeated failures

    refillTokens();

    if (tokens >= amountToConsume)
    {
        tokens -= amountToConsume;
        return true;
    }
    else
    {
        // add fail attempt
        failureTimestamps.push_back(rightNow);

        // remove old fail attempts
        auto blockWindowLimit = rightNow - std::chrono::minutes(blockAttemptWindow);
        while (!failureTimestamps.empty() && failureTimestamps.front() < blockWindowLimit)
            failureTimestamps.pop_front();

        // check if reached threshhold and if so block that ass
        if (failureTimestamps.size() >= blockMaxAttempts)
        {
            blockUntil = rightNow + std::chrono::minutes(blockDuration);
            failureTimestamps.clear();
        }

        return false;
    }
}

std::chrono::seconds TokenBucket::blockTimeLeft()
{
    if (isBlocked())
    {
        auto rightNow = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::seconds>(blockUntil - rightNow);
    }
    return std::chrono::seconds(0);
}

bool TokenBucket::isBlocked()
{
    auto rightNow = std::chrono::steady_clock::now();

    if (rightNow < blockUntil)
        return true;

    return false;
}

bool TokenBucket::cleanUpIfOld(int minutes)
{
    auto rightNow = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(rightNow - lastRefillTime);
    if (std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count() >= minutes * 60)
    {
        return true;
    }
    return false;
}

double TokenBucket::returnTokensLeft()
{
    refillTokens();
    return tokens;
}