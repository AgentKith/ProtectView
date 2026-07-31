.PHONY: all build test clean run deps deps-check

BINARY_NAME := unvr-carousal
BUILD_DIR := ./build
MAIN := ./cmd

# Detect OS and architecture
GOOS ?= $(shell go env GOOS)
GOARCH ?= $(shell go env GOARCH)

all: build

deps:
	@if [ "$(GOOS)" = "linux" ]; then \
		echo "Installing Linux dependencies..."; \
		sudo apt update; \
		sudo apt install -y golang ffmpeg build-essential \
			libgl1-mesa-dev libglu1-mesa-dev libwayland-dev libx11-dev \
			libxcursor-dev libxrandr-dev libxinerama-dev libxi-dev \
			libxkbcommon-dev libxxf86vm-dev; \
	elif [ "$(GOOS)" = "darwin" ]; then \
		echo "Installing macOS dependencies..."; \
		brew install go ffmpeg; \
	else \
		echo "Unsupported platform: $(GOOS)"; \
		exit 1; \
	fi

deps-check:
	@echo "Checking dependencies..."
	@command -v go >/dev/null 2>&1 || { echo "go not found"; exit 1; }
	@command -v ffmpeg >/dev/null 2>&1 || { echo "ffmpeg not found"; exit 1; }
	@command -v gcc >/dev/null 2>&1 || { echo "gcc not found"; exit 1; }
	@echo "Dependencies OK"

build: deps-check
	@mkdir -p $(BUILD_DIR)
	GOOS=$(GOOS) GOARCH=$(GOARCH) go build -o $(BUILD_DIR)/$(BINARY_NAME) $(MAIN)
	@echo "Built $(BUILD_DIR)/$(BINARY_NAME) ($(GOOS)/$(GOARCH))"

# Platform-specific builds
build-linux: deps-check
	GOOS=linux GOARCH=amd64 go build -o $(BUILD_DIR)/$(BINARY_NAME)-linux-amd64 $(MAIN)

build-linux-arm: deps-check
	GOOS=linux GOARCH=arm64 go build -o $(BUILD_DIR)/$(BINARY_NAME)-linux-arm64 $(MAIN)

build-pi: build-linux-arm

test:
	go test ./app/ ./unvr/ ./video/

run: build
	$(BUILD_DIR)/$(BINARY_NAME)

clean:
	rm -rf $(BUILD_DIR)
