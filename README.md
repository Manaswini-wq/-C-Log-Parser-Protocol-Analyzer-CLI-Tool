# Log Protocol Analyzer

A C++ command-line tool for parsing, decoding, filtering, and analyzing embedded communication logs (CAN bus and UART). Decodes raw protocol data into human-readable fields, computes traffic statistics, and exports to CSV/JSON.

## Architecture

```
┌─────────────┐    ┌──────────────┐    ┌──────────────┐    ┌────────────┐
│  CLI Parser  │───>│  Log Parser  │───>│ Filter Engine │───>│  Exporter  │
│  (options)   │    │  (CAN/UART)  │    │  (chain)     │    │ (CSV/JSON) │
└─────────────┘    └──────┬───────┘    └──────┬───────┘    └────────────┘
                          │                    │
                   ┌──────▼───────┐    ┌──────▼───────┐
                   │ CAN Decoder  │    │ Stats Engine │
                   │ UART Decoder │    │ (per-ID)     │
                   └──────────────┘    └──────────────┘
```

## Usage

```bash
# Build
mkdir build && cd build
cmake .. && cmake --build .

# Parse and show stats
./loganalyzer -i ../sample_data/can_log.txt -s

# Decode CAN messages
./loganalyzer -i ../sample_data/can_log.txt -d

# Filter by CAN ID and export to CSV
./loganalyzer -i ../sample_data/can_log.txt --can-id 0x1A0 -o output.csv

# Export as JSON
./loganalyzer -i ../sample_data/can_log.txt -f json -o output.json

# Run tests
./test_all
```

## Features

- **Multi-protocol parsing** — CAN and UART log formats with auto-detection
- **CAN signal decoding** — Bit-level extraction with scale/offset (DBC-style)
- **UART frame detection** — A5 commands, Modbus, HDLC, ASCII text
- **Chainable filters** — Protocol, CAN ID, time range, data byte match
- **Traffic statistics** — Per-ID packet count, interval min/avg/max, throughput timeline
- **Export** — CSV and JSON output formats
- **30+ unit tests** — Parser, decoder, stats, filter coverage

## Tech Stack

| Component | Technology |
|-----------|-----------|
| Language | C++17 |
| Build | CMake 3.14+, CTest |
| Patterns | Strategy (decoders), Builder (filters), Polymorphism (PacketDecoder) |
| Testing | Custom framework, zero dependencies |
