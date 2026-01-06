<div align="center">

# SharePaste
<img src="./src/frontend/favicon/favicon.svg" width="350" />

<!-- <p align="center">
<img alt="Static Badge" src="https://img.shields.io/badge/Docker%20Image%20Size%20-7.5MB-blue?style=flat&logo=docker&link=https%3A%2F%2Fgithub.com%2Fhqo998%2FSharePaste%2Fpkgs%2Fcontainer%2Fsharepaste">
<img alt="Static Badge" src="https://github.com/hqo998/SharePaste/actions/workflows/docker-image-build.yml/badge.svg">
<img alt="Static Badge" src="https://github.com/hqo998/SharePaste/actions/workflows/cmake-multi-platform.yml/badge.svg">

</p> -->

[![Static Badge](https://img.shields.io/badge/Docker%20Image%20Size%20-7.5MB-blue?style=flat&logo=docker&link=https%3A%2F%2Fgithub.com%2Fhqo998%2FSharePaste%2Fpkgs%2Fcontainer%2Fsharepaste)](https://github.com/hqo998/SharePaste/pkgs/container/sharepaste)
[![CMake on multiple platforms](https://github.com/hqo998/SharePaste/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/hqo998/SharePaste/actions/workflows/cmake-multi-platform.yml)
[![Docker Image CI](https://github.com/hqo998/SharePaste/actions/workflows/docker-image-build.yml/badge.svg)](https://github.com/hqo998/SharePaste/actions/workflows/docker-image-build.yml)

---
**Instantly share code** or note snippets with others as a randomly generated URL extension.
</div>

# Features

- **Simple:** Easy to use and setup to get running with minimal manual configuration.
- **Lightweight:** Small image and uses C++ backend to provide lightning fast responses.
- **Text editor:** Tab indentation, wrapping, numberlines, and UTF-8 support.
- **Rate-Limiting:** Built in configurable rate-limiting to minimise spam and abuse with Token-bucket algorithm.
- **Database:** Persistent data across reboots, uses SQLite3 to store entries
- **Docker Image:** Easy to run on nearly any OS or device.
- **Trusted Proxy:** Set trusted IPs to allow forwarded IP headers for reverse proxy.
- **View Counts:** See how many views your pastes get.

# Getting Started
## Docker
```
docker run -d \
	--name sharepaste \
	-v /opt/sharepaste/data:/data \
	-p 8080:8080 \
	ghcr.io/hqo998/sharepaste:latest
```
## Docker Compose
### Quickstart.
```dockerfile
services:
  sharepaste:
    container_name: sharepaste
    image: ghcr.io/hqo998/sharepaste:latest
    ports:
      - 8080:8080
    restart: unless-stopped
    volumes:
      - /opt/sharepaste/data:/data
```
---
### Advanced Configuration
```dockerfile
services:
  sharepaste:
    container_name: sharepaste
    image: ghcr.io/hqo998/sharepaste:latest
    ports:
      - 8080:8080
    restart: unless-stopped
    volumes:
      - /opt/sharepaste/data:/data
    environment:
      - SP_TrustedProxies=192.168.0.1, 192.168.0.2  # Change to your proxy IPs
      - SP_RateLimit_TokenCapacity=10               # Set to 0 to disable rate limiting
      - SP_RateLimit_RefillRate=0.5                 # Refill tokens per second
      - SP_RateLimit_BlockAttemptWindow=5           # Minutes
      - SP_RateLimit_BlockMaxAttempts=10            # Count
      - SP_RateLimit_BlockDuration=10               # Minutes (0 to disable)
      - SP_RateLimit_CleanUpInterval=600            # Seconds
      - SP_RateLimit_CleanMinimumAge=5              # Minutes
      - SP_AdminContactEmail=admin@email            # Displayed email for report contact.
      - SP_RateLimit_MaxPasteSize=100000            # Character/s Limit.
```
---
### With Auto-Update.
```dockerfile
services:
  sharepaste:
    container_name: sharepaste
    image: ghcr.io/hqo998/sharepaste:latest
    ports:
      - 8080:8080
    restart: unless-stopped
    volumes:
      - /opt/sharepaste/data:/data

  watchtower:
    container_name: watchtower
    image: nickfedor/watchtower:latest
    restart: unless-stopped
    volumes:
      - /var/run/docker.sock:/var/run/docker.sock
    environment:
      - WATCHTOWER_POLL_INTERVAL=600
      - WATCHTOWER_CLEANUP=true
      - WATCHTOWER_MONITOR_ONLY=false
      - WATCHTOWER_REVIVE_STOPPED=true
      - WATCHTOWER_INCLUDE_STOPPED=false
      - WATCHTOWER_ENABLE_DIGEST=true
    command: --cleanup
```
## Binaries
> Not available yet. Please build from source with CMake or use Docker.

### Environment Variables

| Variable | Type | Default | Description |
| :--- | :--- | :--- | :--- |
| `SP_TrustedProxies` | Array | - | Comma-separated list of trusted IPs (e.g. `192.168.0.1, 192.168.0.2`). When SharePaste is behind a reverse-proxy and to trust forwarded IP headers. |
| `SP_RateLimit_TokenCapacity` | Integer | `10` | Maximum token bucket size. Set to `0` to disable rate limiting. |
| `SP_RateLimit_RefillRate` | Double | `0.5` | Tokens added to bucket per second. |
| `SP_RateLimit_BlockAttemptWindow` | Integer | `5` | Timeframe window (minutes) to track failed attempts. |
| `SP_RateLimit_BlockMaxAttempts` | Integer | `10` | Max attempts allowed in the window before a block occurs. |
| `SP_RateLimit_BlockDuration` | Integer | `10` | Block duration (minutes). Set to `0` to disable long blocks. |
| `SP_RateLimit_CleanUpInterval` | Integer | `600` | IP Tracker bucket cleanup frequency (seconds). |
| `SP_RateLimit_CleanMinimumAge` | Integer | `5` | Minutes an IP must be idle before bucket removal. |
| `SP_AdminContactEmail` | String | `example@example` | Email to contact for user reports. Shows on the about page. |
| `SP_RateLimit_MaxPasteSize` | Integer | `100000` | Max characters allowed per paste. Frontend limit 5 million in text box. |

## Dependancies
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [SQLite3](https://sqlite.org)
- [Nlohmann JSON](https://github.com/nlohmann/json)
- These are automatically downloaded and configured when built with provided CMAKE
