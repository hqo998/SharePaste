<div align="center">

# SharePaste
<img src="./src/frontend/favicon/favicon.svg" width="450" />

---
Instantly share a code or note snipped with others as a randomly generated URL extension. The backend is made with C++ and using SQLite3 as a database.
</div>


## Installation
### Binaries
> Not available yet. Please build from source.

### Docker
```
docker run -d \
	--name sharepaste \
	-v /opt/sharepaste/data:/data \
	-p 8080:8080 \
	ghcr.io/hqo998/sharepaste:latest
```

### Docker Compose
Quickstart.
```
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
With auto update.
```
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

## Dependancies
- Automatically downloaded and when built with provided CMAKE
- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [SQLite3](https://sqlite.org)
- [Nlohmann JSON](https://github.com/nlohmann/json)
