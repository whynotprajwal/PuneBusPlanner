# Pune Smart Bus Route Planner

## Comprehensive Project Report

| Item | Detail |
|---|---|
| Project title | Pune Smart Bus Route Planner |
| Project type | C++17 console application with a local browser dashboard |
| Project folder | `C:\Users\Prajwal\Desktop\PuneBusPlanner` |
| Primary source | `OOP_CP.cpp` (2,295 lines, 90,098 bytes) |
| Report date | 1 November 2026 |
| Verified environment | Windows, PowerShell 5.1, g++ 16.1.0 (MSYS2), Python 3.14.6, Node.js 24.22.1, Microsoft Edge headless |
| Documentation status | Based on a complete read of the source, data files, web server, frontend, and observed test output |

This report describes the project as it exists in the `PuneBusPlanner` folder. It covers the problem, features, architecture, classes, algorithms, data files, console interface, JSON API, web backend, browser UI, testing, limitations, and possible future work. It is intentionally detailed so that the project can be understood, demonstrated, maintained, and extended without reading every source file first.

---

## Table of Contents

1. [Executive Summary](#1-executive-summary)
2. [Problem Statement, Goals, and Scope](#2-problem-statement-goals-and-scope)
3. [Feature Inventory](#3-feature-inventory)
4. [Technology Stack and Environment](#4-technology-stack-and-environment)
5. [Repository and File Inventory](#5-repository-and-file-inventory)
6. [High-Level Architecture](#6-high-level-architecture)
7. [Pune Network and Data Model](#7-pune-network-and-data-model)
8. [Core C++ Design](#8-core-c-design)
9. [Routing Algorithm](#9-routing-algorithm)
10. [Time, Traffic, and Dwell Model](#10-time-traffic-and-dwell-model)
11. [Fare System](#11-fare-system)
12. [Fleet and Seat Inventory](#12-fleet-and-seat-inventory)
13. [Tickets, Persistence, and Analytics](#13-tickets-persistence-and-analytics)
14. [Console Application](#14-console-application)
15. [JSON API and Web Backend](#15-json-api-and-web-backend)
16. [Browser UI](#16-browser-ui)
17. [OOP and Course-Unit Mapping](#17-oop-and-course-unit-mapping)
18. [Build, Run, and Deployment](#18-build-run-and-deployment)
19. [Testing and Verification](#19-testing-and-verification)
20. [Edge Cases and Error Handling](#20-edge-cases-and-error-handling)
21. [Security and Robustness](#21-security-and-robustness)
22. [Known Limitations and Observed Behaviors](#22-known-limitations-and-observed-behaviors)
23. [Future Enhancements](#23-future-enhancements)
24. [Viva and Interview Questions](#24-viva-and-interview-questions)
25. [Conclusion](#25-conclusion)
26. [Appendices](#26-appendices)

---

## 1. Executive Summary

The Pune Smart Bus Route Planner is a graph-based public transport application. It models Pune bus stops as graph nodes and roads as weighted undirected edges. Four bus lines share four transfer stops, so a journey can pass through more than one line.

The program supports:

- Three route optimization modes: fastest time, shortest distance, and fewest transfers.
- Four bus types: Regular Feeder, Rapid Express, Comfort AC, and Eco Electric.
- Three passenger categories: Regular, Student, and Senior.
- Four fare strategies: RegularFare, ExpressFare, StudentDiscount, and SeniorDiscount.
- Journey planning, itinerary display, fleet comparison, seat booking, cancellation, and trip analytics.
- A styled ANSI-colour console interface with an ASCII fallback.
- A local browser dashboard backed by the same C++ logic through a JSON API.

The central design decision is that routing, fare calculation, fleet behaviour, booking, and persistence are implemented once in C++ and reused by both interfaces. The web UI does not duplicate the planner logic in JavaScript. This keeps the console and browser results consistent.

The current project has 25 stops, 25 roads, 4 lines, and 4 buses. The current engine produces a 29.20 km, 73.7 minute, 2-transfer Pimpri-to-Pune-Airport journey on the Rapid Express Bus at 09:00, with an ExpressFare of Rs. 123.12. The self-tests pass, the API endpoints have been exercised, and all three dashboard tabs have been rendered and visually checked.

---

## 2. Problem Statement, Goals, and Scope

### 2.1 Problem statement

A bus passenger needs to answer several questions:

1. Which sequence of stops and lines gets me from my origin to my destination?
2. Is the fastest route the shortest route, or the route with the fewest line changes?
3. How long will the trip take at this time of day?
4. Which bus type gives the best combination of time, fare, and comfort?
5. How much is the fare for my passenger category?
6. Can I book a seat and receive a ticket?
7. Can I cancel a ticket and have the statistics reflect the cancellation?

The project answers these questions with a graph model, Dijkstra routing over a state that includes the current line, a time-of-day model, strategy-based fares, a fleet comparison view, and a booking workflow.

### 2.2 Project goals

- Preserve the original Object-Oriented Programming assignment structure.
- Model a realistic multi-line bus network with transfer stops.
- Demonstrate graph algorithms, not just menu-driven record keeping.
- Demonstrate inheritance, polymorphism, abstraction, templates, operator overloading, exception handling, file I/O, the STL, and common design patterns.
- Provide a polished console experience.
- Provide a browser experience without reimplementing the core logic.
- Keep the project buildable as one C++17 source file.
- Keep the runtime dependency-free apart from the C++ standard library; the optional web UI uses only the Python standard library.

### 2.3 In scope

- Network loading and validation.
- Route search for three optimization criteria.
- Four bus types with different capacities, speeds, dwell times, rates, and boarding fees.
- Rush-hour time adjustment.
- Fare strategies for passenger and bus category.
- Seat booking, ticket display, cancellation, and log persistence.
- Fleet comparison.
- Trip history and aggregate statistics.
- Console presentation with optional colour.
- A local JSON API and browser dashboard.
- Self-tests and manual verification.

### 2.4 Out of scope

- Live traffic data, GPS, or real-time vehicle tracking.
- Real GTFS feeds or government transit APIs.
- Payment processing.
- User accounts and authentication.
- A production database.
- Mobile native applications.
- A geographic map with coordinates and road geometry.
- Multi-city networks.
- Scheduled timetables and exact departure/arrival times.

---

## 3. Feature Inventory

| Area | Feature | Status |
|---|---|---|
| Network | 25 Pune stops | Implemented |
| Network | 25 undirected roads | Implemented |
| Network | 4 colour-coded lines | Implemented |
| Network | 4 shared transfer stops | Implemented |
| Routing | Fastest-time mode | Implemented |
| Routing | Shortest-distance mode | Implemented |
| Routing | Fewest-transfers mode | Implemented |
| Routing | Dijkstra with line-aware state | Implemented |
| Routing | Route reconstruction and itinerary | Implemented |
| Bus fleet | Regular Feeder Bus | Implemented |
| Bus fleet | Rapid Express Bus | Implemented |
| Bus fleet | Comfort AC Bus | Implemented |
| Bus fleet | Eco Electric Bus | Implemented |
| Fares | RegularFare | Implemented |
| Fares | ExpressFare | Implemented |
| Fares | StudentDiscount | Implemented |
| Fares | SeniorDiscount | Implemented |
| Time | Time-of-day rush multiplier | Implemented |
| Time | Per-stop dwell time | Implemented |
| Booking | Seat inventory | Implemented |
| Booking | Ticket generation | Implemented |
| Booking | Ticket log persistence | Implemented |
| Booking | Cancellation marker | Implemented |
| Analytics | Total trips | Implemented |
| Analytics | Total revenue | Implemented |
| Analytics | Total distance | Implemented |
| Analytics | Busiest route | Implemented |
| Analytics | Recent trips | Implemented |
| Analytics | Cancelled-ticket count | Implemented |
| Console | 7-option menu | Implemented |
| Console | ANSI colour | Implemented |
| Console | `--no-color` and `NO_COLOR` | Implemented |
| Console | ASCII tables and banners | Implemented |
| Console | Input validation | Implemented |
| Console | Self-test mode | Implemented |
| API | `--api` line protocol | Implemented |
| API | Network action | Implemented |
| API | Plan action | Implemented |
| API | Compare action | Implemented |
| API | Book action | Implemented |
| Web | Local Python HTTP server | Implemented |
| Web | Static dashboard serving | Implemented |
| Web | Network overview tab | Implemented |
| Web | Journey planner tab | Implemented |
| Web | Booking card and ticket | Implemented |
| Web | Fleet comparison tab | Implemented |
| Web | Responsive layout | Implemented |
| Web | Dark visual theme | Implemented |

---

## 4. Technology Stack and Environment

### 4.1 Languages and libraries

| Layer | Technology | Notes |
|---|---|---|
| Core application | C++17 | Standard library only |
| Compiler used | g++ 16.1.0 (MSYS2) | `-std=c++17 -O2 -Wall -Wextra` |
| Console styling | ANSI escape codes | Optional, disabled with `--no-color` |
| Web server | Python 3.14.6 | `http.server`, `subprocess`, `json`, `threading` |
| Frontend | HTML5, CSS3, JavaScript | No frameworks, no npm dependencies |
| Testing | `assert`, Node syntax check, Python compile check, API calls, headless Edge | Manual and automated checks |
| Browser used | Microsoft Edge headless | 1440 × 1400 screenshots |

### 4.2 Portability

- The C++ application is a single source file and builds with a standard C++17 compiler.
- The console UI uses ANSI escape sequences; Windows Terminal and modern `conhost` support them.
- `--no-color` and `NO_COLOR=1` make output safe for redirected files, reports, and older terminals.
- The web UI requires a modern browser because it uses `replaceAll`, CSS custom properties, `color-mix`, and modern layout features.
- The local server binds only to `127.0.0.1`, so it is not exposed to the network by default.

---

## 5. Repository and File Inventory

### 5.1 Runtime and source files

| File | Bytes | Lines | Purpose |
|---|---:|---:|---|
| `OOP_CP.cpp` | 90,098 | 2,295 | Complete C++17 application, console UI, tests, and JSON API mode |
| `routes.txt` | 1,828 | 67 | Pune network definition |
| `trip_log.txt` | 267 | 3 | Current booking and cancellation history |
| `server.py` | 6,213 | 206 | Local HTTP server and C++ backend bridge |
| `run_ui.bat` | 210 | 13 | Windows launcher for the web UI |
| `web_ui/index.html` | 4,949 | 151 | Dashboard structure |
| `web_ui/style.css` | 14,623 | 866 | Dashboard styling and responsive layout |
| `web_ui/app.js` | 17,588 | 532 | Dashboard behaviour and API calls |
| `OOP_CP_report.md` | 17,963 | 412 | Earlier detailed technical report |
| `OOP_CP_brief_report.md` | 6,688 | 131 | Short project brief |
| `PROJECT_REPORT.md` | This file | — | Comprehensive report |

### 5.2 Screenshot artifacts

| File | Purpose |
|---|---|
| `screenshots/ui_preview.png` | Network overview tab |
| `screenshots/ui_plan.png` | Journey planner with a calculated route |
| `screenshots/ui_compare.png` | Fleet comparison results |

### 5.3 Runtime-generated files

- `pune_bus_api.exe` is generated in the operating system temporary directory by `server.py`.
- No compiled executable is required in the project folder for the web UI.
- `trip_log.txt` is appended to when a booking is made.
- `trip_log.txt` receives a `CANCEL` line when a current-session ticket is cancelled.

### 5.4 Important note about the current folder

The folder contains source, data, documentation, and screenshots, but no bundled executable. This is intentional:

- `run_ui.bat` builds the API backend automatically.
- The console can be built with `g++` when needed.
- Avoiding a bundled `.exe` keeps the project portable and avoids stale binaries.

---

## 6. High-Level Architecture

### 6.1 Layered structure

```text
+---------------------------------------------------------------+
|                        User Interfaces                        |
|                                                               |
|   Console menu / ANSI tables          Browser dashboard       |
|   OOP_CP.cpp main()                   index.html/style.css/app.js
+---------------------------+-------------------+---------------+
                            |                   |
                            |                   | HTTP JSON
                            |                   v
                            |          +----------------------+
                            |          |      server.py       |
                            |          | ThreadingHTTPServer  |
                            |          | static files + API   |
                            |          +----------+-----------+
                            |                     |
                            |                     | stdin/stdout
                            v                     v
+---------------------------------------------------------------+
|                    OOP_CP.cpp --api mode                      |
|          line-based JSON request / JSON response              |
+-------------------------------+-------------------------------+
                                |
                                v
+---------------------------------------------------------------+
|                     BusNetworkManager                         |
|  network loading | fleet | planning | booking | analytics     |
+----------------+----------------+----------------+--------------+
                 |                |                |
                 v                v                v
        +----------------+ +-------------+ +------------------+
        |    BusGraph    | | Fleet<Bus>  | | Tickets / log    |
        | stops + roads  | | unique_ptr  | | trip_log.txt     |
        | Dijkstra       | | FareStrategy| | statistics       |
        +----------------+ +-------------+ +------------------+
```

### 6.2 Key architectural decisions

1. **Single source of truth.** All routing, fare, fleet, and booking rules live in C++.
2. **Transport is separate from business logic.** `--api` converts JSON to calls on `BusNetworkManager`.
3. **The web server is a bridge, not a second application.** `server.py` does not calculate routes.
4. **The UI is static.** The browser loads HTML/CSS/JS from `web_ui/` and calls local JSON endpoints.
5. **The backend is persistent.** One `OOP_CP.exe --api` process is kept alive while the server runs, so the in-memory seat/ticket state survives multiple HTTP requests.
6. **Requests are serialized.** A lock in `server.py` protects the single stdin/stdout protocol of the C++ backend.

### 6.3 Console request flow

```text
User menu choice
   -> main() validates input
   -> BusNetworkManager method
   -> BusGraph / Bus / FareStrategy / Ticket / file I/O
   -> formatted console output
```

### 6.4 Web plan flow

```text
Browser plan form
   -> POST /api/plan
   -> server.py validates JSON and adds action=plan
   -> C++ --api handleApiRequest()
   -> BusNetworkManager::planJourney()
   -> BusGraph::findRoute() + FareStrategy
   -> buildPlanJson()
   -> HTTP JSON response
   -> app.js renderPlan()
```

### 6.5 Web booking flow

```text
Browser Book seat button
   -> POST /api/book
   -> server.py adds action=book
   -> C++ replans the journey
   -> Bus::reserveSeat()
   -> Ticket created and appended to trip_log.txt
   -> ticket stored in current-session map
   -> buildBookJson()
   -> browser renders ticket and refreshes fleet seats
```

---

## 7. Pune Network and Data Model

### 7.1 Stops

| ID | Stop | ID | Stop |
|---:|---|---:|---|
| 1 | Pimpri | 14 | Katraj |
| 2 | Chinchwad | 15 | Camp |
| 3 | Khadki | 16 | Bund_Garden |
| 4 | Shivajinagar | 17 | Viman_Nagar |
| 5 | Civil_Court | 18 | Kharadi |
| 6 | Swargate | 19 | Pune_Airport |
| 7 | Vanaz | 20 | Hinjawadi |
| 8 | Kothrud | 21 | Wakad |
| 9 | Deccan_Gymkhana | 22 | Baner |
| 10 | PMC | 23 | Aundh |
| 11 | Pune_Station | 24 | Hadapsar |
| 12 | Kalyani_Nagar | 25 | Magarpatta |
| 13 | Ramwadi |  |  |

### 7.2 Lines and roads

#### PurpleLine

| From | To | Distance | Speed limit |
|---|---|---:|---:|
| Pimpri (1) | Chinchwad (2) | 3.0 km | 40 km/h |
| Chinchwad (2) | Khadki (3) | 4.5 km | 35 km/h |
| Khadki (3) | Shivajinagar (4) | 5.0 km | 35 km/h |
| Shivajinagar (4) | Civil_Court (5) | 2.5 km | 30 km/h |
| Civil_Court (5) | Swargate (6) | 2.0 km | 30 km/h |

Line length: 17.0 km. Endpoints: Pimpri and Swargate.

#### AquaLine

| From | To | Distance | Speed limit |
|---|---|---:|---:|
| Vanaz (7) | Kothrud (8) | 4.0 km | 35 km/h |
| Kothrud (8) | Deccan_Gymkhana (9) | 3.5 km | 35 km/h |
| Deccan_Gymkhana (9) | PMC (10) | 2.2 km | 30 km/h |
| PMC (10) | Civil_Court (5) | 1.8 km | 30 km/h |
| Civil_Court (5) | Pune_Station (11) | 2.5 km | 30 km/h |
| Pune_Station (11) | Kalyani_Nagar (12) | 4.0 km | 40 km/h |
| Kalyani_Nagar (12) | Ramwadi (13) | 5.0 km | 45 km/h |

Line length: 23.0 km. Endpoints: Vanaz and Ramwadi.

#### RedLine

| From | To | Distance | Speed limit |
|---|---|---:|---:|
| Katraj (14) | Swargate (6) | 6.5 km | 35 km/h |
| Swargate (6) | Camp (15) | 2.2 km | 30 km/h |
| Camp (15) | Bund_Garden (16) | 2.0 km | 30 km/h |
| Bund_Garden (16) | Viman_Nagar (17) | 3.2 km | 35 km/h |
| Viman_Nagar (17) | Kharadi (18) | 4.5 km | 45 km/h |
| Kharadi (18) | Pune_Airport (19) | 4.0 km | 50 km/h |

Line length: 22.4 km. Endpoints: Katraj and Pune_Airport.

#### GreenLine

| From | To | Distance | Speed limit |
|---|---|---:|---:|
| Hinjawadi (20) | Wakad (21) | 4.0 km | 40 km/h |
| Wakad (21) | Baner (22) | 3.5 km | 35 km/h |
| Baner (22) | Aundh (23) | 3.0 km | 35 km/h |
| Aundh (23) | Shivajinagar (4) | 5.5 km | 40 km/h |
| Shivajinagar (4) | Bund_Garden (16) | 5.0 km | 40 km/h |
| Bund_Garden (16) | Hadapsar (24) | 6.0 km | 45 km/h |
| Hadapsar (24) | Magarpatta (25) | 3.0 km | 40 km/h |

Line length: 30.0 km. Endpoints: Hinjawadi and Magarpatta.

### 7.3 Network totals

| Metric | Value |
|---|---:|
| Stops | 25 |
| Roads | 25 |
| Lines | 4 |
| Directed adjacency entries | 50 |
| Unique road distance | 92.4 km |
| Average road length | 3.696 km |
| Transfer stops | 4 |

### 7.4 Transfer stops

A stop is a transfer point when it belongs to more than one line.

| Stop | Lines | Transfer role |
|---|---|---|
| Shivajinagar (4) | PurpleLine, GreenLine | Pimpri-side Purple to Green/Hadapsar direction |
| Civil_Court (5) | PurpleLine, AquaLine | Purple to Aqua/Vanaz or Pune Station direction |
| Swargate (6) | PurpleLine, RedLine | Purple to Red/Katraj or Airport direction |
| Bund_Garden (16) | RedLine, GreenLine | Red to Green/Hadapsar or Green to Red/Airport |

### 7.5 `routes.txt` format

```text
# STOP <id> <name>
# EDGE <fromId> <toId> <distanceKm> <speedLimitKmh> <lineName>
STOP 1 Pimpri
STOP 2 Chinchwad
...
EDGE 1 2 3.0 40 PurpleLine
EDGE 2 3 4.5 35 PurpleLine
...
```

Rules enforced by the loader:

- Blank lines and lines beginning with `#` are ignored.
- `STOP` must have exactly two fields after the tag: numeric ID and name.
- Stop IDs must be positive and unique.
- Stop names cannot contain spaces; underscores are used.
- `EDGE` must have exactly five fields after the tag.
- Both stops must already exist when the edge is read.
- Distance must be positive and finite.
- Speed limit must be positive, finite, and no greater than 200 km/h.
- Line name must not be empty.
- Self-loops are rejected.
- All `STOP` lines must appear before the `EDGE` lines that use them.
- At least one stop and one edge must exist.
- The loader builds a temporary `BusGraph` and only replaces the active graph after full validation.

Roads are stored as undirected edges. The loader adds both travel directions internally.

### 7.6 `trip_log.txt` format

Each completed booking is one tab-separated line with ten fields:

```text
ticketId  passengerName  passengerType  origin  destination  busType  distanceKm  minutes  fare  fareStrategy
```

A cancellation is:

```text
CANCEL    ticketId
```

There is no header row. The log is append-only.

Current content:

| Ticket | Passenger | Type | Route | Bus | Distance | ETA | Fare | Strategy |
|---|---|---|---|---|---|---:|---:|---:|
| TKT-1001 | Prajwal | Regular | Pimpri → Pune_Airport | Rapid Express Bus | 29.20 km | 73.7 min | Rs. 123.12 | ExpressFare |
| TKT-1002 | prajwal | Senior | Pimpri → Khadki | Comfort AC Bus | 7.50 km | 16.0 min | Rs. 29.40 | SeniorDiscount |
| TKT-1003 | prajwal | Senior | Pimpri → Khadki | Rapid Express Bus | 7.50 km | 14.8 min | Rs. 26.25 | SeniorDiscount |

There are no `CANCEL` lines in the project log at the time of this report. Historical ETA values are read as stored; they are not recalculated from today's formula.

---

## 8. Core C++ Design

### 8.1 Source map

| Lines | Section | Responsibility |
|---:|---|---|
| 1–21 | Includes | Standard library headers |
| 26–31 | `trimCopy()` | Trim whitespace |
| 37–131 | `Style` and presentation | ANSI colour, tables, banners, menu formatting |
| 133–168 | Enums | `OptimizationMode`, `PassengerType`, `BusKind` and parsers |
| 170–213 | Exceptions | Domain-specific exception classes |
| 215–247 | `RouteStop` | Stop ID, name, legacy distance, stream output |
| 249–318 | `Bus` | Abstract bus base class and seat inventory |
| 320–383 | Concrete buses | Regular, Express, AC, Electric |
| 385–399 | `BusFactory` | `unique_ptr<Bus>` creation |
| 401–461 | Fare hierarchy | Context, strategy interface, concrete strategies, factory |
| 463–575 | `Passenger`, `Ticket` | Passenger record, ticket text, log line, ID sync |
| 577–829 | Graph and routing | `Road`, `RouteResult`, `BusGraph`, Dijkstra |
| 831–845 | `RouteOptimizer<T>` | Template cumulative distance |
| 847–1544 | `BusNetworkManager` | Loading, fleet, planning, booking, cancellation, analytics |
| 1546–1573 | Input helpers | Checked numeric input and line input |
| 1575–1710 | `runSelfTests()` | Assert-based tests |
| 1712–2086 | JSON API mode | Silencer, JSON helpers, builders, request handler, loop |
| 2088–2295 | `main()` | Flags, menu, error handling |

### 8.2 Utility and presentation layer

- `trimCopy()` removes leading and trailing spaces, tabs, and newlines.
- `Style` is a namespace containing an `enabled` flag and ANSI colour constants.
- `Style::paint()` returns plain text when colour is disabled.
- `fitRight()` and `fitLeft()` pad or truncate text to a requested width. Their names describe the padding direction rather than conventional text alignment.
- `numberString()` formats floats with fixed precision.
- `lineColor()` maps line names to colours.
- `printTitle()`, `printSubtitle()`, `printRule()`, `printKV()`, `printTableSeparator()`, `printTableRow()`, and `printMenuItem()` provide consistent console layout.

### 8.3 Enum classes

```cpp
enum class OptimizationMode { FASTEST = 1, SHORTEST = 2, FEWEST_TRANSFERS = 3 };
enum class PassengerType { REGULAR = 1, STUDENT = 2, SENIOR = 3 };
enum class BusKind { REGULAR = 1, EXPRESS = 2, AC = 3, ELECTRIC = 4 };
```

Using `enum class` avoids accidental integer conversions and name collisions. Helper functions convert values to display strings, and parsers validate numeric input.

### 8.4 Exceptions

| Exception | Prefix in `what()` | Used for |
|---|---|---|
| `RouteNotFoundException` | `Route Error:` | Unreachable route, file open/create failure, log write failure |
| `InvalidStopException` | `Input Error:` | Unknown stop ID |
| `InvalidInputException` | `Invalid Input:` | Bad mode, bad passenger type, malformed file, invalid bus choice |
| `BusFullException` | `Booking Error:` | Seat reservation on a full bus |

All four derive from `std::exception` and override `what() const noexcept`.

### 8.5 `RouteStop`

| Member | Type | Purpose |
|---|---|---|
| `stopId` | `int` | Unique stop identifier |
| `stopName` | `string` | Stop name |
| `distanceFromPrevious` | `float` | Legacy compatibility field; not used by graph routing |

Methods:

- Default constructor.
- Parameterized constructor using `this->`.
- `getId()`, `getName()`, `getDistance()`.
- Friend `operator<<` that temporarily changes stream formatting and restores the original flags.

### 8.6 `Bus`

`Bus` is an abstract base class.

**Protected data:**

| Member | Meaning |
|---|---|
| `busId` | Fleet identifier such as `REG-101` |
| `busType` | Human-readable bus type |
| `seatCapacity` | Total seats |
| `seatsBooked` | Seats currently reserved in this session |
| `baseSpeed` | Bus speed in km/h |
| `dwellMinutes` | Minutes added per intermediate stop |
| `fareRatePerKm` | Fare rate |
| `boardingFee` | Fixed boarding charge |
| `totalBusesActive` | Static count of live bus objects |

**Public interface:**

- Parameterized constructor with validation of capacity, speed, and dwell.
- Deleted copy constructor, copy assignment, move constructor, and move assignment.
- Virtual destructor decrementing the active-bus counter.
- Pure virtual `calculateTripTime(distance, roadLimit, rushMultiplier)`.
- Virtual `isExpressBus()` returning `false` by default.
- Getters for ID, type, capacity, booked seats, available seats, base speed, dwell, fare rate, and boarding fee.
- `reserveSeat()` throwing `BusFullException` when full.
- `releaseSeat()` decrementing only when seats are booked.
- Static `getActiveBusCount()`.

**Why non-copyable and non-movable:** a copied bus would not run the constructor but its destructor could still decrement the static live-bus counter. Deleting copy and move operations keeps construction and destruction balanced. The fleet owns buses through `unique_ptr`.

### 8.7 Concrete bus classes

| Class | Type string | ID | Capacity | Base speed | Dwell | Rate/km | Boarding | Rush adjustment |
|---|---|---:|---:|---:|---:|---:|---:|---|
| `RegularBus` | Regular Feeder Bus | REG-101 | 60 | 30 km/h | 3.5 min | Rs. 2.00 | Rs. 10 | Full rush multiplier |
| `ExpressBus` | Rapid Express Bus | EXP-202 | 45 | 55 km/h | 1.0 min | Rs. 3.00 | Rs. 15 | 65% of rush increase |
| `ACBus` | Comfort AC Bus | AC-303 | 40 | 45 km/h | 2.0 min | Rs. 3.20 | Rs. 18 | 75% of rush increase |
| `ElectricBus` | Eco Electric Bus | ELE-404 | 50 | 40 km/h | 1.5 min | Rs. 1.80 | Rs. 8 | 60% of rush increase |

`ExpressBus` is the only class overriding `isExpressBus()` to return `true`.

### 8.8 `BusFactory`

`BusFactory::create(BusKind)` returns `std::unique_ptr<Bus>`:

| Kind | Object |
|---|---|
| `REGULAR` | `RegularBus("REG-101")` |
| `EXPRESS` | `ExpressBus("EXP-202")` |
| `AC` | `ACBus("AC-303")` |
| `ELECTRIC` | `ElectricBus("ELE-404")` |

Unknown values throw `InvalidInputException`.

### 8.9 Fare hierarchy

`FareContext` contains:

- `distanceKm`
- `ratePerKm`
- `boardingFee`

`FareStrategy` is an abstract interface with:

- `virtual string name() const`
- `virtual float calculate(const FareContext&) const`
- Virtual destructor

Concrete strategies:

| Class | Name | Formula |
|---|---|---|
| `RegularFare` | `RegularFare` | `boarding + distance × rate` |
| `ExpressFare` | `ExpressFare` | `(boarding + distance × rate) × 1.20` |
| `StudentDiscount` | `StudentDiscount` | `(boarding + distance × rate) × 0.50` |
| `SeniorDiscount` | `SeniorDiscount` | `(boarding + distance × rate) × 0.70` |

`FareStrategyFactory::create(passengerType, expressBus)` selects:

1. Student → `StudentDiscount`
2. Senior → `SeniorDiscount`
3. Regular + express bus → `ExpressFare`
4. Regular + non-express bus → `RegularFare`

Consequence: a student or senior using an express bus receives the student/senior discount on that bus's base rate, not an additional express multiplier. The express multiplier only applies to regular passengers.

### 8.10 `Passenger`

| Member | Type |
|---|---|
| `passengerId` | `int` |
| `passengerName` | `string` |
| `category` | `PassengerType` |

Getters expose the ID, name, and type.

### 8.11 `Ticket`

**Data:**

- Static `nextSerial` starting at 1000.
- `ticketId` in the form `TKT-n`.
- Passenger object.
- Bus ID and bus type.
- Origin and destination names.
- Distance, minutes, fare, and fare strategy name.

**Methods:**

- Constructor increments `nextSerial`.
- `toLogLine()` produces the ten-field tab-separated record.
- `syncSerialFromLog(filename)` scans the log for the highest `TKT-` serial and advances `nextSerial` so IDs do not repeat across restarts.
- Friend `operator<<` prints a formatted ticket and temporarily uses `Style` helpers without changing global stream flags.

### 8.12 Graph types

`Road`:

| Field | Type |
|---|---|
| `fromId` | `int` |
| `toId` | `int` |
| `distanceKm` | `float` |
| `speedLimitKmh` | `float` |
| `lineName` | `string` |

`RouteResult`:

| Field | Type | Meaning |
|---|---|---|
| `found` | `bool` | Whether a route exists |
| `stopIds` | `vector<int>` | Ordered stop sequence |
| `legs` | `vector<Road>` | Ordered road segments |
| `transfers` | `int` | Number of line changes |
| `totalDistanceKm` | `float` | Sum of road distances |
| `totalMinutes` | `float` | Estimated journey time |
| `message` | `string` | Status or error text |

### 8.13 `BusGraph`

**Private data:**

- `map<int, RouteStop> stops`
- `map<int, vector<Road>> adjacency`
- `size_t roadCountValue`

**Validation in `addStop`:**

- Positive ID.
- Non-empty name.
- No duplicate ID.

**Validation in `addRoad`:**

- Both stop IDs must exist.
- No self-loop.
- Positive finite distance.
- Speed limit greater than zero and at most 200.
- Non-empty line name.
- Adds both directions.
- Increments the unique road count.

**Read-only access:**

- `hasStop()`
- `getStop()`
- `getStops()`
- `getAdjacency()`
- `stopCount()`
- `roadCount()`
- `getLines()` returning a sorted unique list from a `set`.

**Routing entry point:**

- `findRoute(startId, endId, mode, bus, rushMultiplier)`

### 8.14 `RouteOptimizer<T>`

A small template class with a static method:

```cpp
computeCumulativeDistance(const vector<T>& distances, int startIdx, int endIdx)
```

It is used in `planJourney()` to recompute the total route distance from the leg distances. This demonstrates generic programming even though the route result already contains the total distance.

### 8.15 `JourneyPlan`

| Field | Purpose |
|---|---|
| `valid` | Whether planning succeeded |
| `route` | `RouteResult` |
| `busIndex` | Zero-based fleet index |
| `rushMultiplier` | Applied rush multiplier |
| `fare` | Calculated fare |
| `fareStrategy` | Strategy name |
| `passengerType` | Passenger category |
| `originName` | Origin name |
| `destinationName` | Destination name |

### 8.16 `BusNetworkManager`

**Owned state:**

- `BusGraph graph`
- `vector<unique_ptr<Bus>> fleet`
- `map<string, Ticket> tickets` for current-session active tickets
- Static `totalQueriesProcessed`
- Static `nextPassengerId`

**Construction sequence:**

1. Load `routes.txt`, creating it if missing or empty.
2. Synchronize ticket IDs from `trip_log.txt`.
3. Create four buses through `BusFactory`.

**Public methods:**

| Method | Purpose |
|---|---|
| `getGraph()` | Read-only graph access for API builders |
| `getFleet()` | Read-only fleet access for API builders |
| `loadNetworkFromFile()` | Validate and replace network |
| `rushMultiplierForHour()` | Convert hour to rush factor |
| `displayNetwork()` | Print stops, lines, roads |
| `displayFleet()` | Print fleet table |
| `planJourney()` | Validate, route, calculate fare, print summary |
| `bookTicket()` | Reserve seat, create ticket, append log |
| `cancelTicket()` | Release seat, append cancellation |
| `compareFleet()` | Compare all bus types for a route |
| `showTripHistory()` | Read log, aggregate and show statistics |
| `showSystemStats()` | Show query, fleet, ticket, and network counts |

### 8.17 JSON/API support classes and functions

| Item | Purpose |
|---|---|
| `ScopedCoutSilencer` | Temporarily redirects `std::cout` into a string buffer while the C++ console functions run inside API mode |
| `jsonEscape()` | Escapes JSON string content |
| `jsonGetRaw()` | Minimal extraction of a flat JSON value by key |
| `jsonGetInt()` | Integer extraction with fallback |
| `buildNetworkJson()` | Serializes network and fleet |
| `buildPlanJson()` | Serializes a plan result |
| `buildCompareJson()` | Serializes fleet comparison |
| `buildBookJson()` | Serializes a booked ticket |
| `handleApiRequest()` | Dispatches `network`, `plan`, `compare`, `book` |
| `runApiMode()` | Reads one JSON request per line and writes one JSON response per line |

### 8.18 `main()`

- Reads `NO_COLOR`.
- Parses `--no-color`, `--test`, and `--api`.
- `--api` takes precedence over `--test` because it is checked first.
- Runs the API loop, self-tests, or the interactive menu.
- Constructs `BusNetworkManager` inside a `try` block.
- Catches domain exceptions inside the menu loop and startup exceptions outside it.
- Handles end-of-input gracefully.

---

## 9. Routing Algorithm

### 9.1 Why Dijkstra

Roads have positive distance and time weights. Dijkstra's algorithm guarantees a minimum-cost path when edge costs are non-negative. Breadth-first search would only be correct for unweighted graphs, and depth-first search does not guarantee an optimal route.

### 9.2 State space

A normal shortest-path state is `(stop)`. That is not enough here because the cost of the next road depends on whether it changes line. The project therefore uses:

```text
state = (stopId, currentLine)
```

- The start state is `(startId, "")`.
- The first road never counts as a transfer.
- Moving from a road on line `A` to a road on line `B` adds one transfer.
- The same stop can be visited multiple times with different current lines.
- The destination is any state whose stop ID equals the destination, and the best among those states is selected.

### 9.3 Priority-queue item

```cpp
struct PQItem {
    float primary;
    float secondary;
    int node;
    string line;
    float distance;
    float time;
    int transfers;
};
```

Ordering:

1. Lower primary cost first.
2. Lower secondary cost next.
3. Lower node ID as a deterministic tie-breaker.

### 9.4 Cost definitions by mode

| Mode | Primary cost | Secondary cost | Meaning |
|---|---|---|---|
| Fastest | Time in minutes | Distance | Minimize travel time, then distance |
| Shortest | Distance in km | Time in minutes | Minimize distance, then time |
| Fewest transfers | Transfer count | Distance | Minimize line changes, then distance |

### 9.5 Algorithm outline

```text
best[(start, "")] = zero-cost state
push (start, "") into priority queue

while queue is not empty:
    pop best candidate
    skip stale entry if it is worse than the best known state
    for each road from current stop:
        extraTransfer = 1 if current line is non-empty and differs from road line, else 0
        newTransfers = current transfers + extraTransfer
        legTime = bus.calculateTripTime(road distance, road speed limit, rush)
        if mode == FASTEST: legTime += bus dwell
        newDistance = current distance + road distance
        newTime = current time + legTime
        choose primary/secondary for mode
        if (road destination, road line) is better:
            store predecessor node, predecessor line, via road, totals
            push new state

choose best state whose stop ID is the destination
reconstruct roads by walking predecessor links backward and reversing
```

### 9.6 Route reconstruction

The `best` map stores for each state:

- Primary and secondary cost.
- Previous node.
- Previous line.
- Previous road.
- Distance, time, transfers.
- A `hasPrev` flag.

Reconstruction starts at the best destination state, walks backward until the start state, collects roads, and reverses the list. Stop IDs are then built from the start ID and each leg's destination.

### 9.7 Complexity

Let:

- `V` = number of stops (25)
- `E` = number of directed adjacency entries (50)
- `L` = number of distinct lines (4)

The state space is `O(V × (L + 1))`. Each state can relax its outgoing roads. The general complexity is:

```text
O(E × L × log(V × L))
```

For the current network this is very small: roughly 100 states at most. The implementation also uses `std::map` for storage, so practical constants are larger than a vector-based graph, but well within the required scale.

### 9.8 Actual mode comparison for Pimpri to Pune_Airport

At 09:00 with the Rapid Express Bus:

| Mode | Distance | Time | Transfers | Fare | Route summary |
|---|---:|---:|---:|---:|---|
| Fastest | 29.20 km | 73.7 min | 2 | Rs. 123.12 | Purple → Green → Red |
| Shortest Distance | 29.20 km | 73.7 min | 2 | Rs. 123.12 | Purple → Green → Red |
| Fewest Transfers | 32.90 km | 91.8 min | 1 | Rs. 136.44 | Purple → Red via Swargate |

This is a useful demonstration: the fewest-transfers route is longer and slower, but it avoids one line change.

### 9.9 Self-test route example

The self-tests build a synthetic graph:

```text
A(1) --1.0 km, 20 km/h, SlowLine-- B(2) --1.0 km, 20 km/h, SlowLine-- C(3)
A(1) --2.0 km, 100 km/h, FastLine-- X(6) --2.0 km, 100 km/h, FastLine-- C(3)
Island(4) is disconnected
Y(7) is unused
```

The tests confirm that shortest-distance chooses `A → B → C` and fastest chooses `A → X → C`.

---

## 10. Time, Traffic, and Dwell Model

### 10.1 Rush multiplier

`BusNetworkManager::rushMultiplierForHour(hour)`:

| Hour range | Multiplier | Label |
|---|---:|---|
| 7–10 and 17–20 | 1.80 | RUSH |
| 11–16 | 1.20 | BUSY |
| 0–6 and 21–23 | 0.85 | OFF-PEAK |

The multiplier affects travel time. It does **not** affect fare.

### 10.2 Effective speed

```text
effective speed = max(5, min(bus base speed, road speed limit) / rushAdjusted)
```

The 5 km/h floor prevents division by extremely small values.

Rush adjustment by bus type:

| Bus | Rush adjustment formula |
|---|---|
| Regular | `max(0.5, rushMultiplier)` |
| Express | `1 + (max(0.5, rushMultiplier) - 1) × 0.65` |
| AC | `1 + (max(0.5, rushMultiplier) - 1) × 0.75` |
| Electric | `1 + (max(0.5, rushMultiplier) - 1) × 0.60` |

The Express, AC, and Electric buses dampen the rush multiplier, so their travel time grows less than the Regular bus.

### 10.3 Trip time

```text
trip minutes = (distance / effectiveSpeed) × 60
```

### 10.4 Dwell time

Dwell is the time spent at intermediate stops.

- In fastest mode, each road adds one bus-specific dwell interval during the search.
- After route reconstruction, one dwell is subtracted because the final stop is not an intermediate stop.
- In shortest and fewest-transfers modes, the search does not add dwell to edge costs. The final ETA adds `intermediateStops × dwell`.

This means the displayed ETA includes dwell for all modes, but the fastest-mode search explicitly optimizes with dwell included. In shortest/fewest modes, the secondary time used during search does not include dwell; this is a small modelling nuance rather than a routing failure.

### 10.5 Actual 09:00 comparison

For Pimpri to Pune_Airport, 29.2 km:

| Bus | ETA | Fare for Regular passenger |
|---|---:|---:|
| Regular Feeder | 126.1 min | Rs. 68.40 |
| Rapid Express | 73.7 min | Rs. 123.12 |
| Comfort AC | 84.2 min | Rs. 111.44 |
| Eco Electric | 77.9 min | Rs. 60.56 |

### 10.6 Observed time-of-day effect

Rapid Express, Pimpri to Pune_Airport:

| Hour | Rush factor | Label | ETA | Fare |
|---:|---:|---|---:|---:|
| 06:00 | 0.85 | OFF-PEAK | 46.2 min | Rs. 123.12 |
| 09:00 | 1.80 | RUSH | 73.7 min | Rs. 123.12 |
| 13:00 | 1.20 | BUSY | 56.4 min | Rs. 123.12 |
| 18:00 | 1.80 | RUSH | 73.7 min | Rs. 123.12 |
| 22:00 | 0.85 | OFF-PEAK | 46.2 min | Rs. 123.12 |

The fare is constant because the fare model uses distance, rate, and boarding fee only. The ETA changes because the effective speed changes.

---

## 11. Fare System

### 11.1 Formula

```text
base = boardingFee + distanceKm × ratePerKm
```

Then the strategy multiplier is applied.

| Strategy | Multiplier | Applies to |
|---|---:|---|
| RegularFare | 1.00 | Regular passenger, non-express bus |
| ExpressFare | 1.20 | Regular passenger, express bus |
| StudentDiscount | 0.50 | Student passenger |
| SeniorDiscount | 0.70 | Senior passenger |

### 11.2 Strategy selection precedence

The factory checks passenger category first:

```text
STUDENT  -> StudentDiscount
SENIOR   -> SeniorDiscount
REGULAR  -> ExpressFare if bus is express, else RegularFare
```

Therefore, a student on a Rapid Express Bus gets a student discount on the express bus's higher base rate, not a 1.20 express multiplier stacked on a student discount.

### 11.3 Fare examples

For 29.20 km:

| Bus | Base calculation | Regular | Student | Senior |
|---|---|---:|---:|---:|
| Regular Feeder | 10 + 29.2 × 2.00 = 68.40 | Rs. 68.40 | Rs. 34.20 | Rs. 47.88 |
| Rapid Express | 15 + 29.2 × 3.00 = 102.60 | Rs. 123.12 | Rs. 51.30 | Rs. 71.82 |
| Comfort AC | 18 + 29.2 × 3.20 = 111.44 | Rs. 111.44 | Rs. 55.72 | Rs. 78.01 |
| Eco Electric | 8 + 29.2 × 1.80 = 60.56 | Rs. 60.56 | Rs. 30.28 | Rs. 42.39 |

The displayed values are rounded to two decimal places. The internal values are floats.

### 11.4 Historical log examples

- TKT-1002: Comfort AC, 7.5 km, Senior: `(18 + 7.5 × 3.2) × 0.70 = 29.40`.
- TKT-1003: Rapid Express, 7.5 km, Senior: `(15 + 7.5 × 3.0) × 0.70 = 26.25`.
- TKT-1001: Rapid Express, 29.2 km, Regular: `(15 + 29.2 × 3.0) × 1.20 = 123.12`.

---

## 12. Fleet and Seat Inventory

### 12.1 Fleet composition

The fleet is created in this order:

1. Regular Feeder Bus, `REG-101`
2. Rapid Express Bus, `EXP-202`
3. Comfort AC Bus, `AC-303`
4. Eco Electric Bus, `ELE-404`

### 12.2 Seat state

- Each bus starts with `seatsBooked = 0`.
- `getAvailableSeats()` returns `capacity - seatsBooked`.
- `reserveSeat()` increments the counter or throws `BusFullException`.
- `releaseSeat()` decrements only when at least one seat is booked.
- Fleet ownership is through `unique_ptr`; no manual `new` or `delete`.
- The static active-bus count is incremented in the constructor and decremented in the destructor.
- Buses are non-copyable and non-movable to keep the counter safe.

### 12.3 Booking algorithm

```text
validate plan
trim passenger name; reject empty
find selected bus
reserve a seat
create Passenger with a new passenger ID
create Ticket with a new ticket ID
append ticket line to trip_log.txt
store ticket in the active ticket map
if anything after seat reservation fails:
    release the seat
    rethrow
```

The log write is checked for open success and write success. If logging fails, the seat is released.

One nuance: `nextPassengerId` and the ticket serial are incremented before the log write. A failed booking can therefore consume an ID even though the booking is rolled back. This is harmless for uniqueness but can create gaps.

### 12.4 Cancellation algorithm

```text
look up ticket in the current-session active-ticket map
if absent: throw InvalidInputException
find the bus by ticket bus ID
release one seat
append CANCEL <ticketId> to trip_log.txt
remove ticket from active map
```

Cancellation is limited to tickets created in the current process. After a restart, historical tickets are in the log but not in the in-memory map, so they cannot be cancelled through the menu. The log still records previous cancellations and statistics exclude them.

### 12.5 Seat reset behaviour

Seat inventory is session-local. The constructor creates fresh buses with zero booked seats. It does not replay historical bookings from `trip_log.txt`. Therefore:

- A fresh process shows all buses as fully available.
- Ticket IDs still continue correctly because the log is scanned.
- Trip history and revenue still include historical bookings unless cancelled.

This behaviour is important for interpreting the dashboard: a bus can show 45/45 seats even though there are older bookings in the log.

---

## 13. Tickets, Persistence, and Analytics

### 13.1 Ticket ID generation

- Static `Ticket::nextSerial` starts at 1000.
- Constructor increments it, so the first ticket is `TKT-1001`.
- `syncSerialFromLog("trip_log.txt")` scans every line beginning with `TKT-`, parses the numeric serial, and keeps the maximum.
- Current log maximum is 1003; the next new ticket in a fresh process would be `TKT-1004`.

### 13.2 Log persistence

- Bookings append one tab-separated line.
- Cancellations append `CANCEL\t<ticketId>`.
- The log is not rewritten; it is append-only.
- Statistics parse the log on demand.
- Malformed or short lines are ignored by the analytics parser.
- Negative parsed distance, minutes, or fare are ignored.

### 13.3 History and statistics algorithm

1. Read all non-empty log lines.
2. Collect every ticket ID that appears after `CANCEL\t`.
3. Ignore lines that do not have exactly ten fields.
4. Ignore cancelled ticket IDs.
5. Parse distance, minutes, and fare with `stod`.
6. Skip invalid or negative values.
7. Increment trip count, total distance, and revenue.
8. Count each route as `origin -> destination`.
9. Choose the route with the highest count as busiest.
10. Show the five most recent non-cancelled trips in reverse log order.

### 13.4 Current project statistics

Observed with the current `trip_log.txt`:

| Statistic | Value |
|---|---:|
| Total trips | 3 |
| Total revenue | Rs. 178.77 |
| Busiest route | Pimpri → Khadki (2 trips) |
| Total distance | 44.2 km |
| Cancelled tickets | 0 |
| Recent trips | TKT-1003, TKT-1002, TKT-1001 |

### 13.5 Cancellation test in an isolated copy

An isolated test copy of the project (in the temporary directory) booked `TKT-1004`, then cancelled it. Results:

- Booking succeeded.
- Cancellation released the seat.
- A `CANCEL TKT-1004` line was appended.
- Statistics still showed 3 completed trips and excluded the cancelled trip.
- `Cancelled Tickets` changed to 1.
- A later booking in a new process received `TKT-1005`, proving serial synchronization.
- Seat availability returned to full, confirming session-local seat inventory.

The project's own `trip_log.txt` was not modified by this isolated test.

### 13.6 System analytics

The console option **System Statistics** shows:

- Planning queries served in the current session.
- Active fleet objects in memory.
- Active tickets in the current session.
- Network size: stops, roads, lines.

In a fresh process the observed values were:

```text
Planning Queries Served  : 0
Active Fleets in Memory  : 4
Active Tickets           : 0
Network Size             : 25 stops, 25 roads, 4 lines
```

`totalQueriesProcessed` is incremented by console planning and console comparison. API planning also calls `planJourney()` and increments it, but the API comparison builder does not. This is an internal metric only, not a routing correctness issue.

---

## 14. Console Application

### 14.1 Menu

```text
[1] View Route Network & Fleet
[2] Plan Journey (Dijkstra + Fare + Booking)
[3] Compare All Bus Types
[4] Cancel Ticket
[5] Trip History & Statistics
[6] System Statistics
[7] Exit System
```

### 14.2 Option behaviour

| Option | Behaviour |
|---|---|
| 1 | Prints all stops, line paths with road distances/speeds, and fleet table |
| 2 | Prints network/fleet, asks for origin, destination, mode, hour, passenger type, bus, then plans; optionally books |
| 3 | Asks for origin, destination, mode, hour, passenger type; prints all bus options |
| 4 | Asks for a current-session ticket ID and cancels it |
| 5 | Reads `trip_log.txt` and prints statistics and recent trips |
| 6 | Prints session and network analytics |
| 7 | Exits |

### 14.3 Command-line flags

| Flag / environment | Effect |
|---|---|
| `--no-color` | Disables ANSI colour |
| `NO_COLOR=1` | Disables ANSI colour |
| `--test` | Runs assert-based self-tests and exits |
| `--api` | Runs the line-based JSON API and exits after input closes |
| `--api --test` | API mode wins because it is checked first |

### 14.4 Input handling

- `readNumber<T>()` uses `cin >>` and checks success.
- On failure it calls `cin.clear()` and discards the rest of the line.
- `askNumber()` prints a styled prompt and delegates to `readNumber()`.
- `readLineValue()` uses `getline`, trims the result, and throws if the stream is closed.
- Numeric input with trailing junk may be accepted as the numeric prefix because extraction stops at the first non-numeric character; the remainder is discarded.
- End-of-input exits the menu cleanly.

### 14.5 Error handling

| Error type | Console prefix |
|---|---|
| Full bus | `[BOOKING FAILED]` |
| Invalid input | `[WARNING]` |
| Unknown stop | `[STOP ERROR]` |
| Route/file problem | `[SYSTEM ERROR]` |
| Unexpected exception | `[UNEXPECTED ERROR]` |
| Startup failure | `[FATAL STARTUP ERROR]` to stderr |

### 14.6 Presentation

- Titles are 64 characters wide.
- Colour is enabled by default.
- Tables use `+`, `-`, and `|`.
- Line names are colour-coded by substring: Purple, Aqua, Red, Green.
- Rush labels are colour-coded: RUSH red, BUSY yellow, OFF-PEAK green.
- `operator<<` restores stream flags so later output is not affected.
- `--no-color` produces plain ASCII suitable for reports.

### 14.7 Representative itinerary

```text
START 1. Pimpri
      | PurpleLine  3.0 km  40 km/h
STOP  2. Chinchwad
      | PurpleLine  4.5 km  35 km/h
STOP  3. Khadki
      | PurpleLine  5.0 km  35 km/h
      | TRANSFER at Shivajinagar
STOP  4. Shivajinagar
      | GreenLine  5.0 km  40 km/h
      | TRANSFER at Bund_Garden
STOP  5. Bund_Garden
      | RedLine  3.2 km  35 km/h
STOP  6. Viman_Nagar
      | RedLine  4.5 km  45 km/h
STOP  7. Kharadi
      | RedLine  4.0 km  50 km/h
END   8. Pune_Airport
```

---

## 15. JSON API and Web Backend

### 15.1 Why an API mode

The browser cannot call C++ classes directly. The `--api` mode provides a small line-based protocol:

- One JSON object per input line.
- One JSON object per output line.
- The C++ process stays alive.
- `server.py` owns the process and forwards HTTP requests.

This avoids duplicating routing and fare logic in JavaScript.

### 15.2 API request format

Example:

```json
{"action":"plan","start":1,"dest":19,"mode":1,"hour":9,"passenger":1,"bus":2}
```

The C++ parser is intentionally minimal. It supports flat JSON objects and simple scalar values. It is not a general-purpose JSON parser.

### 15.3 Actions

#### `network`

Request:

```json
{"action":"network"}
```

Response shape:

```json
{
  "ok": true,
  "network": {
    "stops": [{"id": 1, "name": "Pimpri"}],
    "lines": ["AquaLine", "GreenLine", "PurpleLine", "RedLine"],
    "roads": [{
      "from": 1,
      "to": 2,
      "fromName": "Pimpri",
      "toName": "Chinchwad",
      "distance": 3.0,
      "speed": 40,
      "line": "PurpleLine"
    }],
    "fleet": [{
      "index": 1,
      "type": "Regular Feeder Bus",
      "id": "REG-101",
      "seatsBooked": 0,
      "capacity": 60,
      "available": 60,
      "fareRate": 2.00,
      "boardingFee": 10.00,
      "speed": 30,
      "express": false
    }]
  }
}
```

The `roads` array includes only roads where `from < to`, so each undirected road appears once in the UI.

#### `plan`

Request fields:

| Field | Type | Required | Meaning |
|---|---|---|---|
| `start` | int | yes | Origin stop ID |
| `dest` | int | yes | Destination stop ID |
| `mode` | int | yes | 1 fastest, 2 shortest, 3 fewest transfers |
| `hour` | int | yes | 0–23 |
| `passenger` | int | yes | 1 regular, 2 student, 3 senior |
| `bus` | int | yes | Fleet index 1–4 |

Response fields:

| Field | Meaning |
|---|---|
| `origin`, `destination` | Names |
| `mode` | Display name |
| `hour` | Requested hour |
| `busIndex`, `bus`, `busType`, `busId` | Selected bus |
| `rush`, `rushLabel` | Time multiplier and label |
| `distance`, `minutes` | Route totals |
| `transfers` | Line changes |
| `intermediateStops` | Stops between origin and destination |
| `fare`, `fareStrategy`, `passengerType` | Fare result |
| `availableSeats`, `capacity` | Current seat state |
| `stops` | Ordered stop objects with `id`, `name`, `tag` |
| `legs` | Ordered road objects with `from`, `to`, names, line, distance, speed |

#### `compare`

Request fields: `start`, `dest`, `mode`, `hour`, `passenger`.

Response:

```json
{
  "ok": true,
  "compare": {
    "mode": "Fastest",
    "hour": 9,
    "rush": 1.80,
    "passengerType": "Regular",
    "options": [{
      "bus": "Regular Feeder Bus",
      "busId": "REG-101",
      "found": true,
      "distance": 29.2,
      "minutes": 126.1,
      "transfers": 2,
      "fare": 68.40,
      "strategy": "RegularFare"
    }]
  }
}
```

If a bus has no route, the option contains only `bus`, `busId`, and `"found": false`.

#### `book`

Request fields are the same as `plan`, plus:

| Field | Type | Meaning |
|---|---|---|
| `name` | string | Passenger name; defaults to `Passenger` if blank |

Response:

```json
{
  "ok": true,
  "ticket": {
    "id": "TKT-1004",
    "passenger": "Test User",
    "passengerType": "Regular",
    "bus": "Rapid Express Bus [EXP-202]",
    "route": "Pimpri -> Pune_Airport",
    "distance": 29.20,
    "minutes": 73.7,
    "fare": 123.12,
    "fareStrategy": "ExpressFare",
    "availableSeats": 44
  }
}
```

The booking action replans the journey from the same inputs. It is not idempotent: a retry creates another ticket and reserves another seat.

### 15.4 Error response

```json
{"ok": false, "error": "Invalid Input: Unknown API action: 'wat'."}
```

Errors are caught in `runApiMode()` and serialized with `jsonEscape()`.

### 15.5 HTTP endpoints exposed by `server.py`

| Method | Path | Action |
|---|---|---|
| GET | `/api/network` | `network` |
| POST | `/api/plan` | `plan` |
| POST | `/api/compare` | `compare` |
| POST | `/api/book` | `book` |
| GET | `/` and other static paths | Serves `web_ui/` |

The server strips query strings before routing. Invalid JSON returns 400 with a JSON body. A domain error returned by C++ becomes HTTP 400. An internal bridge failure becomes HTTP 500. JSON responses include `Cache-Control: no-store`.

### 15.6 `server.py` lifecycle

1. Resolve `BASE_DIR`, `WEB_DIR`, `SOURCE`, and temporary `API_EXE`.
2. Read `PORT` from the environment or default to 8000.
3. If the API executable is missing or older than `OOP_CP.cpp`, compile it with g++.
4. Start the API process with `--api`.
5. Send a `network` handshake to verify the backend is alive.
6. Start a `ThreadingHTTPServer` bound to `127.0.0.1`.
7. Print the URL.
8. Open the browser after 0.8 seconds unless `NO_BROWSER=1`.
9. On exit, close the HTTP server and stop the API process.

The server registers `stop_api` with `atexit` and also calls it in the `finally` block. `stop_api` closes stdin, terminates the process, waits up to three seconds, and kills it if necessary.

### 15.7 Compilation performed by the server

```text
g++ -std=c++17 -O2 -Wall -Wextra -o <temp>/pune_bus_api.exe <project>/OOP_CP.cpp
```

If g++ is missing, the server exits with a clear message. If compilation fails, it reports the exit code.

### 15.8 Backend process communication

- `subprocess.Popen` with `text=True`, line buffering.
- One request is written as compact JSON plus a newline.
- One response line is read back.
- `api_lock` serializes all calls from the threaded HTTP server.
- stderr is captured; if the backend exits unexpectedly, the server attempts to include stderr in the error message.

### 15.9 Environment variables

| Variable | Default | Effect |
|---|---|---|
| `PORT` | 8000 | HTTP port |
| `NO_BROWSER` | unset | `1` suppresses automatic browser opening |
| `NO_COLOR` | unset | Read by the C++ console application, not by the web UI |

### 15.10 Why the web UI uses a temp executable

In this environment, Direct execution of a freshly built `.exe` in the Downloads folder was blocked by Windows Device Guard. Building `pune_bus_api.exe` in the operating system temporary directory avoided that problem. `server.py` does this automatically. The project folder therefore does not need a local `.exe` for the web UI.

---

## 16. Browser UI

### 16.1 Files

| File | Role |
|---|---|
| `web_ui/index.html` | Page structure, tabs, forms, result containers, status pill, toast |
| `web_ui/style.css` | Dark theme, cards, tables, timeline, responsive rules |
| `web_ui/app.js` | State, API calls, rendering, event handling, helpers |

### 16.2 Page structure

- Header with brand mark, title, subtitle, and backend status pill.
- Tab navigation: Network, Plan Journey, Compare Fleet.
- Network panel:
  - Overview heading and Refresh button.
  - Four statistics cards.
  - Line cards with stop dots, transfer markers, segment distances.
  - Fleet table.
- Plan Journey panel:
  - Form with origin, destination, mode, hour, passenger, bus.
  - Find Route button.
  - Summary metric cards.
  - Trip card with metadata chips and a vertical timeline.
  - Booking card with passenger name and Book Seat button.
  - Ticket slot.
- Compare Fleet panel:
  - Form with origin, destination, mode, hour, passenger.
  - Comparison summary.
  - Table with best time and best fare highlighted.
- Toast notification area.

### 16.3 JavaScript state

```js
const state = {
  network: null,
  lastPlan: null,
};
```

- `network` holds the latest network/fleet snapshot.
- `lastPlan` holds the last successful plan request body and result, used by the booking action.

### 16.4 Initialization

`init()`:

1. Binds tab buttons.
2. Binds Refresh.
3. Binds Plan form submit.
4. Binds Compare form submit.
5. Binds Book button.
6. Loads the network.
7. Activates a tab from the URL hash.

### 16.5 API helper

`api(path, body)`:

- Uses GET when there is no body.
- Uses POST with JSON when there is a body.
- Parses the response JSON.
- Throws on invalid JSON, non-OK HTTP status, or `ok: false`.
- The UI surfaces the error through a toast.

### 16.6 Network loading

`loadNetwork()`:

- Fetches `/api/network`.
- Stores `data.network`.
- Renders network and fleet.
- Populates stop and bus selectors when requested.
- Sets status to "Backend connected" or "Backend offline".

`populateSelectors()`:

- Sorts stops by ID.
- Preserves previous selections when possible.
- Defaults the destination to stop 19 (Pune Airport) if needed.
- Builds bus options from the fleet.
- Preserves the selected bus if possible.

### 16.7 Line rendering

`buildLinePaths(network)`:

1. Maps stop IDs to names.
2. Groups roads by line.
3. Builds adjacency per line.
4. Finds line endpoints.
5. Traverses each line from an endpoint, marking visited edges.
6. Collects ordered stops and segments.

`renderLines()`:

- Counts how many line paths contain each stop.
- Marks stops appearing more than once as transfers.
- Renders a horizontal line track with dots, transfer emphasis, and segment distance labels.

This works for the current network because each line is a simple path. A branching line would require a different visualization strategy.

### 16.8 Fleet rendering

`renderFleet()` shows:

- Fleet index.
- Bus type and ID.
- Available seats over capacity.
- Rate per km.
- Express or Standard class.

After a booking, `onBook()` calls `loadNetwork({populate:false})` to refresh seat counts without resetting the plan form.

### 16.9 Plan rendering

`renderPlan(plan)`:

- Shows distance, estimated time, transfers, and fare.
- Shows a trip header with origin and destination.
- Shows chips for mode, bus, passenger type, rush label/multiplier, and seats.
- Shows the fare strategy tag.
- Builds a vertical timeline from `plan.stops` and `plan.legs`.
- Adds a transfer note when the incoming line differs from the outgoing line.
- Colours each line using the line colour mapping.

### 16.10 Booking

`onBook()`:

- Requires a previous plan.
- Requires a non-empty passenger name.
- Sends the last plan body plus the name to `/api/book`.
- Renders the ticket.
- Clears the name field.
- Shows a success toast.
- Refreshes fleet seat counts.

`renderTicket()` shows ticket ID, passenger and category, bus, route, distance, ETA, fare, and strategy.

### 16.11 Comparison

`onCompare()`:

- Rejects identical origin and destination.
- Posts to `/api/compare`.
- Finds the minimum time and minimum fare among found options.
- Adds a `best` class to the matching cells.
- Shows mode, hour, rush factor, and passenger type.
- Shows a table with bus, distance, time, transfers, fare, and strategy.

### 16.12 Helper functions

| Helper | Purpose |
|---|---|
| `withBusy(button, task)` | Disables a button, shows "Working...", restores state |
| `setStatus(text, stateName)` | Updates the backend status pill |
| `showToast(message, type)` | Shows a temporary notification |
| `lineColor(line)` | Known line colours plus hash-based fallback |
| `formatLine(line)` | Converts CamelCase line names to spaced names |
| `displayName(name)` | Replaces underscores with spaces |
| `escapeHtml(value)` | Escapes HTML-sensitive characters |

### 16.13 Visual design

- Dark navy background with radial cyan, violet, and green glows.
- Glass-like cards with translucent panels and large shadows.
- Gradient primary buttons and active tabs.
- Four-column metric grids that collapse to two and then one column.
- Horizontal scrolling for wide tables and line tracks.
- Timeline with coloured markers and line badges.
- Green emphasis for successful tickets and best values.
- Red/yellow/green rush indicators.
- Mobile breakpoints at 900 px and 620 px.
- Form focus rings and disabled-button styling.

### 16.14 Accessibility notes

Positive:

- Form controls are wrapped in labels.
- Buttons have text.
- Tables use `thead` and `th`.
- Navigation has an `aria-label`.
- Status is conveyed with text, not only colour.
- Focus styles are present.

Limitations:

- Tabs do not implement full ARIA `tablist`/`tab`/`tabpanel` semantics.
- Arrow-key navigation between tabs is not implemented.
- No `prefers-reduced-motion` rule for the panel animation.
- Some small text and low-contrast muted text may be hard to read.
- Toast messages are visual and not announced through an ARIA live region.

### 16.15 Browser compatibility

The UI assumes a modern evergreen browser. It uses:

- `String.prototype.replaceAll`
- CSS custom properties
- `color-mix()`
- CSS grid and flexbox
- `backdrop-filter`
- `history.replaceState`

Older browsers, including Internet Explorer, are not supported.

---

## 17. OOP and Course-Unit Mapping

| Unit / concept | Implementation |
|---|---|
| UNIT 1: Classes and objects | `RouteStop`, `Bus`, `Passenger`, `Ticket`, `BusGraph`, `BusNetworkManager` |
| UNIT 1: Encapsulation | Private data with public getters; manager owns graph/fleet/tickets |
| UNIT 1: Constructors and `this` | Parameterized constructors, `this->` in `RouteStop` |
| UNIT 2: Operator overloading | `RouteStop::operator<<`, `Ticket::operator<<` |
| UNIT 2: Stream formatting | `iomanip`, tables, restored stream flags |
| UNIT 3: Inheritance | Four concrete bus types derive from `Bus` |
| UNIT 3: Runtime polymorphism | Virtual `calculateTripTime`, `isExpressBus`, virtual destructor |
| UNIT 3: Abstract classes | `Bus` and `FareStrategy` have pure virtual functions |
| UNIT 4: Exception handling | Four custom exceptions, try/catch at menu and API boundaries |
| UNIT 4: Templates | `RouteOptimizer<T>`, `readNumber<T>`, `askNumber<T>` |
| UNIT 5: File handling | `routes.txt` loading, `trip_log.txt` append/read |
| UNIT 5: Stream state | `clear()`, `ignore()`, `getline()`, `ios::app` |
| UNIT 5: Validation | File syntax, stop IDs, distances, speeds, hours, choices |
| UNIT 6: Enums | `OptimizationMode`, `PassengerType`, `BusKind` |
| UNIT 6: Strategy pattern | Fare strategies |
| UNIT 6: Factory pattern | `BusFactory`, `FareStrategyFactory` |
| UNIT 7: Graph algorithms | Weighted undirected graph, Dijkstra, state-space search |
| UNIT 7: STL | `vector`, `map`, `set`, `pair`, `priority_queue`, `unique_ptr` |
| UNIT 7: Complexity awareness | State-space reasoning and priority-queue ordering |
| UNIT 8: Smart pointers | `unique_ptr<Bus>`, `unique_ptr<FareStrategy>` |
| UNIT 8: RAII | Fleet and strategies clean up automatically |
| UNIT 8: Rule of Five | `Bus` deletes copy/move operations; virtual destructor |
| UNIT 8: Const correctness | Const getters and const references |
| UNIT 8: Portability | `--no-color`, `NO_COLOR`, ASCII fallback |
| UNIT 8: Integration | JSON API, Python bridge, browser dashboard |

### 17.1 Original-structure preservation

The project keeps the original assignment vocabulary and roles:

- `RouteStop`
- `Bus`
- `RegularBus`
- `ExpressBus`
- `RouteOptimizer<T>`
- `BusNetworkManager`
- custom exception classes
- console menu
- `UNIT 1–5` annotations in the source

It extends the original with `ACBus`, `ElectricBus`, fare strategies, graph routing, booking persistence, analytics, and the web layer.

### 17.2 Fixes and improvements over a typical first version

1. `BusNetworkManager` is constructed inside the startup `try` block.
2. `InvalidInputException` is separate from `InvalidStopException`.
3. Every numeric input read is checked.
4. Route-file validation is comprehensive.
5. `Bus` is non-copyable and non-movable.
6. `operator<<` preserves stream flags.
7. No manual `new`/`delete`; `unique_ptr` is used.
8. Cancelled bookings are excluded from statistics.
9. Colour is optional.
10. The network is realistic Pune data rather than a small sample.
11. Numeric prompts share `askNumber()`.
12. The same C++ logic powers both console and web interfaces.

---

## 18. Build, Run, and Deployment

### 18.1 Prerequisites

- Windows PowerShell or Command Prompt.
- A C++17 compiler. The verified compiler is g++ 16.1.0 from MSYS2.
- Python 3 for the web dashboard.
- A modern browser.
- Optional: Node.js for JavaScript syntax checking.

### 18.2 Change to the project folder

```powershell
cd "C:\Users\Prajwal\Desktop\PuneBusPlanner"
```

### 18.3 Build the console application

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -o OOP_CP.exe OOP_CP.cpp
```

### 18.4 Run the console application

```powershell
.\OOP_CP.exe
.\OOP_CP.exe --no-color
.\OOP_CP.exe --test
.\OOP_CP.exe --api
```

### 18.5 Run the self-tests

```powershell
.\OOP_CP.exe --test
```

Expected output:

```text
All assert-based self-tests passed.
```

### 18.6 Run the web dashboard

Easiest:

```powershell
.\run_ui.bat
```

Or directly:

```powershell
python server.py
```

Then open:

```text
http://127.0.0.1:8000
```

### 18.7 Run on a different port

PowerShell:

```powershell
$env:PORT = "8765"
python server.py
```

Command Prompt:

```bat
set PORT=8765
python server.py
```

### 18.8 Suppress automatic browser opening

PowerShell:

```powershell
$env:NO_BROWSER = "1"
python server.py
```

Command Prompt:

```bat
set NO_BROWSER=1
python server.py
```

### 18.9 First-run behaviour

- If `routes.txt` is missing or empty, the program writes the Pune seed network.
- If `trip_log.txt` does not exist, a booking creates it.
- `server.py` compiles `%TEMP%\pune_bus_api.exe` if it is missing or older than the source.
- No network/internet access is required.

### 18.10 Troubleshooting

| Problem | Likely cause | Solution |
|---|---|---|
| `g++` not found | Compiler not on PATH | Install MSYS2/MinGW and add `g++` to PATH |
| Python not found | Python not installed or not on PATH | Install Python 3 and enable PATH |
| Port already in use | Another process uses 8000 | Set `PORT` to another value |
| Backend offline | C++ API process failed | Run `python server.py` in a terminal and read the error |
| Compilation error in UI | C++ source error | Fix source; server recompiles on next start |
| Direct `.exe` blocked | Policy/Device Guard | Use the web UI or build/run from a permitted location |
| No route shown | Identical or invalid stops | Choose two different valid stops |
| Seats appear full/reset | Session-local inventory | Restart creates a fresh fleet; historical log is separate |
| Ticket not cancellable after restart | Active tickets are session-only | Cancel in the same session; history remains in the log |
| Browser shows stale data | Cached page | Refresh, or restart server and reload |

---

## 19. Testing and Verification

### 19.1 Compilation

Verified command:

```text
g++ -std=c++17 -O2 -Wall -Wextra -o %TEMP%\pune_report_test.exe OOP_CP.cpp
```

Result:

- Exit code 0.
- No compiler warnings.
- No compiler errors.

### 19.2 Self-tests

`--test` output:

```text
All assert-based self-tests passed.
```

Coverage:

| Test area | Assertions verified |
|---|---|
| Regular ETA | Positive result |
| Express ETA | Positive and faster than Regular |
| Rush effect | Regular trip time increases under rush |
| AC ETA | Positive result |
| Electric ETA | Positive result |
| Regular fare | Positive result |
| Express fare | Greater than regular |
| Student fare | Less than regular |
| Senior fare | Less than regular |
| Student vs Senior | Student is cheaper |
| Senior vs Express | Senior is cheaper |
| Same origin/destination | Found, one stop, zero distance, zero transfers |
| Unreachable stop | Not found |
| Invalid stop | Not found |
| Shortest vs fastest | Different synthetic routes chosen |
| Transfer counting | One transfer on a two-line route |
| Duplicate stop | Exception thrown |
| Negative distance | Exception thrown |
| Self-loop | Exception thrown |
| Full bus | `BusFullException` thrown |
| Seat release | Availability increases by one |

Note: `assert` is disabled if compiled with `-DNDEBUG`. The documented builds do not define it.

### 19.3 API verification

Observed results:

| Test | Result |
|---|---|
| Network | 25 stops, 25 roads, 4 lines, 4 fleet entries |
| Plan Pimpri → Pune_Airport, fastest, 09:00, Regular, Express | 29.20 km, 73.7 min, 2 transfers, Rs. 123.12, ExpressFare, 8 stops, 7 legs, 45/45 seats |
| Plan Pimpri → Khadki, fastest, 09:00, Student, AC | 7.50 km, 21.5 min, 0 transfers, Rs. 21.00, StudentDiscount |
| Plan Hinjawadi → Hadapsar, fewest transfers, 18:00, Senior, Electric | 27.00 km, 69.5 min, 0 transfers, Rs. 39.62, SeniorDiscount |
| Compare Pimpri → Pune_Airport, 09:00, Regular | Four found options with expected time/fare differences |
| Unknown action | JSON error `Unknown API action: 'wat'.` |

### 19.4 Compare verification

At 09:00, Pimpri → Pune_Airport, Regular:

| Bus | Distance | Time | Transfers | Fare | Strategy |
|---|---:|---:|---:|---:|---|
| Regular Feeder | 29.2 km | 126.1 min | 2 | Rs. 68.40 | RegularFare |
| Rapid Express | 29.2 km | 73.7 min | 2 | Rs. 123.12 | ExpressFare |
| Comfort AC | 29.2 km | 84.2 min | 2 | Rs. 111.44 | RegularFare |
| Eco Electric | 29.2 km | 77.9 min | 2 | Rs. 60.56 | RegularFare |

The browser highlights the minimum time and minimum fare. In this example, Rapid Express is fastest and Eco Electric is cheapest.

### 19.5 Server verification

Verified from the Desktop project folder:

- Server started successfully.
- Root page returned HTTP 200.
- Root HTML size: 4,949 bytes.
- `/api/network` returned 25 stops and 4 lines.
- C++ backend built and responded.
- No unhandled exception was reported.

### 19.6 Frontend syntax verification

- `node --check web_ui/app.js` passed.
- Python compile check of `server.py` passed.
- No pycache or build artifacts were left in the project folder by the verification.

### 19.7 Headless UI verification

All three tabs were screenshotted at 1440 × 1400:

- Network overview: line cards, transfer markers, fleet table.
- Plan Journey: default form and calculated Pimpri → Pune Airport result with route timeline.
- Compare Fleet: four bus options with best time and best fare highlights.

Screenshots are stored in `screenshots/`.

### 19.8 Isolated booking and cancellation test

Test setup:

- Copied `routes.txt` and `trip_log.txt` to a temporary directory.
- Ran the compiled console application there.
- Planned and booked a Regular passenger on the Rapid Express Bus.
- Cancelled the new ticket.
- Reviewed history and the log.

Observed:

- Booking created `TKT-1004`.
- Cancellation appended `CANCEL TKT-1004`.
- History excluded the cancelled trip.
- `Cancelled Tickets` increased.
- A later process created `TKT-1005`, proving ID synchronization.
- New processes showed full seats, confirming session-local seat state.

### 19.9 Current project log verification

The project log was read and the console history option was run:

```text
Total Trips              : 3
Total Revenue            : Rs. 178.77
Busiest Route            : Pimpri -> Khadki (2 trips)
Total Distance           : 44.2 km
Cancelled Tickets        : 0
```

---

## 20. Edge Cases and Error Handling

### 20.1 Network and file edge cases

| Situation | Behaviour |
|---|---|
| `routes.txt` missing | Seed file is created |
| `routes.txt` empty | Seed file is created |
| Unknown directive | `InvalidInputException` |
| STOP with too few fields | `InvalidInputException` |
| STOP with extra fields | `InvalidInputException` |
| Non-numeric stop ID | `InvalidInputException` |
| Duplicate stop ID | `InvalidInputException` |
| Empty stop name | `InvalidInputException` |
| EDGE with unknown stop | `InvalidInputException` |
| EDGE self-loop | `InvalidInputException` |
| Zero or negative distance | `InvalidInputException` |
| Speed outside 1–200 | `InvalidInputException` |
| Empty line name | `InvalidInputException` |
| No stops | `InvalidInputException` |
| No roads | `InvalidInputException` |
| File cannot open | `RouteNotFoundException` |

### 20.2 Planning edge cases

| Situation | Behaviour |
|---|---|
| Origin unknown | `InvalidStopException` |
| Destination unknown | `InvalidStopException` |
| Origin equals destination | `InvalidInputException` |
| Mode not 1–3 | `InvalidInputException` |
| Hour outside 0–23 | `InvalidInputException` |
| Passenger type not 1–3 | `InvalidInputException` |
| Bus index out of range | `InvalidInputException` |
| No route | `RouteNotFoundException` |
| Disconnected graph | Route result `found = false` |
| Invalid graph-level stop query | Route result `found = false` |

### 20.3 Booking edge cases

| Situation | Behaviour |
|---|---|
| Invalid plan | `InvalidInputException` |
| Empty passenger name | `InvalidInputException` |
| Bus full | `BusFullException`, seat count unchanged |
| Log cannot open | `RouteNotFoundException`, seat rollback |
| Log write fails | `RouteNotFoundException`, seat rollback |
| Ticket not in current session | `InvalidInputException` |
| Ticket bus missing | `InvalidInputException` |
| Cancel log write fails | No explicit error; ticket is still removed from the active map |
| Retry a booking request | Creates another booking; the API is not idempotent |

### 20.4 Input edge cases

- End-of-input exits the menu.
- Non-numeric numeric prompts produce a warning.
- A numeric prefix followed by junk may be accepted.
- Empty ticket ID is rejected.
- Empty passenger name is rejected in the console and in the API book handler defaults to `Passenger`.
- The UI rejects identical origin and destination before calling the API.

### 20.5 API and server edge cases

| Situation | Behaviour |
|---|---|
| Unknown API action | `ok: false` error |
| Invalid HTTP JSON | 400 with JSON error |
| Non-object JSON | 400 with JSON error |
| Unknown HTTP API path | 404 |
| Backend not running | 500 with error |
| Backend exits | Error with stderr if available |
| Port in use | Server exits with a message |
| Missing `web_ui` | Server exits with a message |
| Missing source | Server exits with a message |
| Missing g++ | Server exits with a message |
| Compile failure | Server exits with exit code |
| `NO_BROWSER=1` | Browser is not opened |

---

## 21. Security and Robustness

### 21.1 Positive measures

- The HTTP server binds to `127.0.0.1`, not `0.0.0.0`.
- Dynamic values inserted into HTML are escaped with `escapeHtml()`.
- API errors are returned as JSON rather than crashing the server.
- The C++ API catches exceptions per request and across startup.
- The server validates that posted JSON is an object.
- The server serializes access to the single C++ backend.
- No secrets, credentials, or external services are used.
- No arbitrary code execution or shell commands are exposed through the API.
- The route loader validates file structure and numeric values.
- Fleet ownership is via `unique_ptr`, reducing leak risk.
- Buses are non-copyable, preventing counter corruption.
- Ticket log writes are checked in the booking path.

### 21.2 Security limitations

- No authentication or authorization.
- No CORS policy.
- No HTTPS; local HTTP only.
- No request-size limit.
- No rate limiting.
- The JSON parser is minimal and not hardened for adversarial input.
- The server is not a production WSGI/ASGI server.
- Local malicious processes could interact with the server.
- The app is intended for a single local user.

---

## 22. Known Limitations and Observed Behaviors

This section collects behaviours that are intentional, incidental, or worth documenting.

### 22.1 Data and network

1. Roads are undirected; there are no one-way roads.
2. Stop names cannot contain spaces in `routes.txt`; underscores are used.
3. There are no geographic coordinates, so the UI cannot draw a real map.
4. There are no scheduled departure times or timetables.
5. There are no walking transfers or waiting-time penalties.
6. A transfer is modelled as a line change at a shared stop, with no separate wait time.
7. The network is static and loaded at startup; editing `routes.txt` requires a restart.
8. Duplicate roads are not explicitly rejected.
9. `RouteStop::distanceFromPrevious` is retained for compatibility but unused by graph routing.
10. `RouteOptimizer<T>` is used mostly to demonstrate templates; the route result already contains the total distance.

### 22.2 Routing and time

1. Dijkstra is correct for non-negative costs but the model is not a full transit schedule.
2. Fastest mode includes dwell per edge during search and subtracts one dwell afterward.
3. Shortest/fewest modes add dwell only after route reconstruction; their secondary time during search excludes dwell.
4. Ties are broken by secondary cost and node ID, which makes output deterministic but may not reflect real-world preference.
5. No alternative route ranking is shown; only the single best route per mode.
6. No path is shown for every mode side by side except through separate requests/comparisons.

### 22.3 Fares

1. Fares are distance-based plus boarding fee.
2. Rush hour changes time, not fare.
3. Student and senior discounts are simple multipliers.
4. ExpressFare applies only to regular passengers.
5. No luggage, tax, pass, or group fares.
6. Floating-point arithmetic is used; values are formatted to two decimals.

### 22.4 Booking and persistence

1. Seat inventory is session-local and resets on restart.
2. Historical bookings are not replayed to reserve seats.
3. Active tickets are session-only; old tickets cannot be cancelled after restart.
4. Cancellation log write failure is not explicitly checked or rolled back.
5. A failed booking can consume a passenger ID and ticket serial.
6. The log is append-only; there is no compaction.
7. There is no database, indexing, or concurrent multi-user booking.
8. Booking through the API is not idempotent; retries can double-book.
9. The `book` action replans the route from the submitted inputs rather than using a stored plan token.
10. The UI keeps the last plan in memory; changing the form without replanning does not affect the book action, which uses the last planned snapshot.

### 22.5 Server and API

1. The C++ JSON parser handles only flat objects and simple scalars.
2. It does not support nested objects, arrays as request values, `null`, booleans as inputs, or `\uXXXX` decoding.
3. It searches flat text for keys and does not validate the complete JSON structure; it is not a general-purpose or hardened parser.
4. The server is a single-backend bridge and serializes requests.
5. No HTTPS, authentication, CORS, rate limiting, or request-size limit.
6. No structured logging; request logs are suppressed.
7. Backend stderr is captured but not continuously drained.
8. The API is local-only by design.

### 22.6 UI

1. Requires a modern browser.
2. Line rendering assumes each line is a simple path.
3. No real map or geographic view.
4. No full ARIA tab semantics or arrow-key tab navigation.
5. No `prefers-reduced-motion` handling.
6. Toasts are not announced through an ARIA live region.
7. Tables use horizontal scrolling on narrow screens.
8. No dark/light theme switch.
9. No offline mode beyond the local server.
10. The UI relies on the backend for all calculations.

### 22.7 Environment

1. Direct execution of a downloaded `.exe` was blocked by Device Guard in this environment.
2. The web server avoids that by using a temporary executable.
3. The console application still needs a permitted location from which to run.
4. ANSI colour depends on terminal support.

---

## 23. Future Enhancements

### 23.1 Code organization

- Split `OOP_CP.cpp` into headers and source files:
  - `exceptions.hpp`
  - `models.hpp/.cpp`
  - `fares.hpp/.cpp`
  - `graph.hpp/.cpp`
  - `bus_factory.hpp/.cpp`
  - `ui.hpp/.cpp`
  - `manager.hpp/.cpp`
  - `api.hpp/.cpp`
  - `main.cpp`
  - `tests.cpp`
- Add CMake or a Makefile.
- Add a proper test framework such as Catch2 or GoogleTest.
- Add formatting and static analysis to CI.

### 23.2 Persistence and booking

- Persist active bookings and seat counts across restarts.
- Load and replay historical bookings into fleet state, or store seat state separately.
- Add cancellation of historical tickets.
- Add refund rules and cancellation windows.
- Use SQLite for trips, tickets, and seat inventory.
- Make booking idempotent with request IDs.
- Add multi-user concurrency and locking.
- Add a booking history view per passenger.

### 23.3 Routing and transit realism

- Add scheduled departures and arrival times.
- Add waiting time and transfer walking time.
- Add one-way roads and directed edges.
- Add multiple route alternatives.
- Add GTFS import.
- Add live traffic and vehicle positions.
- Add walking connections between nearby stops.
- Add fare zones and distance slabs.
- Add time-dependent edge costs.

### 23.4 UI and UX

- Add a real map with stop coordinates.
- Add an interactive network graph.
- Add route comparison charts.
- Add dark/light theme toggle.
- Add keyboard navigation and full ARIA tab semantics.
- Add reduced-motion support.
- Add a mobile-first layout.
- Add saved journeys and passenger profiles.
- Add a live backend status indicator with retry.
- Add request cancellation and loading skeletons.
- Add localization and accessibility testing.

### 23.5 API and deployment

- Replace the handwritten JSON parser with a proper JSON library.
- Add CORS, authentication, and rate limiting if exposed beyond localhost.
- Add request size limits and validation.
- Add HTTPS for non-local deployment.
- Add a production server and packaging.
- Add OpenAPI documentation.
- Add structured logging and health checks.
- Add an end-to-end test suite with Playwright or Selenium.

---

## 24. Viva and Interview Questions

**1. Why use Dijkstra instead of BFS or DFS?**

Roads have non-negative weights in distance and time. Dijkstra finds the minimum-cost path. BFS works only for unweighted graphs, and DFS does not guarantee an optimal path.

**2. Why does the Dijkstra state include the current line?**

The cost of the next road depends on whether it changes line. A state of only `(stop)` cannot tell whether a transfer occurred. `(stop, line)` preserves that information and allows transfer counting.

**3. How is a transfer counted?**

The start state has an empty line. The first road does not count as a transfer. For each subsequent road, if its line differs from the current line, the transfer count increases by one.

**4. How is the best destination selected?**

The search may reach the destination with several different current lines. The code scans all states for the destination and selects the one with the best primary and secondary cost.

**5. What is the complexity?**

The state space is `O(V × L)`, where `V` is stops and `L` is lines. The general cost is `O(E × L × log(V × L))`. The current network is small enough that performance is not a concern.

**6. Why use a priority queue?**

It efficiently extracts the lowest-cost state, which is the core of Dijkstra. The custom comparator orders by primary cost, secondary cost, and node ID.

**7. What is the Strategy pattern here?**

`FareStrategy` defines a common interface. `RegularFare`, `ExpressFare`, `StudentDiscount`, and `SeniorDiscount` are interchangeable algorithms. The booking code does not need to know which concrete strategy is used.

**8. What is the Factory pattern here?**

`BusFactory` creates the correct bus subclass. `FareStrategyFactory` creates the correct fare strategy from passenger type and bus class.

**9. Why is `Bus` non-copyable and non-movable?**

A copied bus would not run the constructor logic but its destructor could decrement the static live-bus counter. Deleting copy and move operations keeps the counter correct and forces ownership through `unique_ptr`.

**10. How is dwell time modelled?**

Each intermediate stop adds a bus-specific dwell interval. Fastest routing adds one dwell per edge during search and subtracts one for the final stop. Other modes add `intermediateStops × dwell` after reconstruction.

**11. Why does the fastest route sometimes have more transfers?**

Because the objective is time, not line changes. A faster path with two transfers can beat a slower path with one transfer. The fewest-transfers mode optimizes a different objective.

**12. Why is the fare unchanged by rush hour?**

The fare model uses boarding fee, distance, bus rate, and passenger discount. Rush hour affects effective speed and ETA, not price.

**13. How are ticket IDs kept unique across restarts?**

`Ticket::syncSerialFromLog()` reads `trip_log.txt`, finds the highest `TKT-` serial, and initializes `nextSerial` from it.

**14. How are cancelled trips excluded from statistics?**

`showTripHistory()` first collects ticket IDs from `CANCEL` lines, then skips those IDs when computing revenue, distance, trips, and recent history.

**15. Why are seats not restored after restart?**

The constructor creates a fresh fleet and does not replay historical bookings. This is a deliberate simplification and a documented limitation, not an accidental persistence mechanism.

**16. How does the web UI reuse the C++ logic?**

The browser calls `server.py`, which forwards JSON to a persistent `OOP_CP.exe --api` process. The C++ process invokes the same `BusNetworkManager` methods used by the console.

**17. Why suppress `cout` in API mode?**

Functions such as `planJourney()` print human-readable summaries. If those writes went to stdout, they would corrupt the JSON protocol. `ScopedCoutSilencer` redirects `cout` during those calls and restores it afterward.

**18. Why not write the HTTP server in C++?**

Using Python's standard library keeps the C++ application focused on algorithms and OOP. The server is a small, replaceable bridge, and Python is already available in the environment.

**19. How does the server handle concurrent requests?**

The HTTP server is threaded, but `api_lock` serializes calls to the single C++ backend. This is sufficient for a local single-user dashboard.

**20. How was the project tested?**

It was compiled with warnings enabled, self-tests were run, JavaScript and Python syntax were checked, API requests were exercised, the server was started from the final folder, all three UI tabs were rendered in headless Edge, and booking/cancellation was tested in an isolated copy.

**21. What would you improve next?**

Persist seat state and active tickets, add a proper JSON library and test framework, add a real map, model waiting/transfer time, split the source into modules, and improve accessibility.

---

## 25. Conclusion

The Pune Smart Bus Route Planner is a complete demonstration of applying Object-Oriented Programming and graph algorithms to a practical transport problem. It combines a validated Pune network, line-aware Dijkstra routing, a realistic fleet model, strategic fare calculation, seat booking, persistent trip history, analytics, and a polished browser dashboard.

The strongest design point is the separation between business logic and presentation:

- The console is a rich text interface.
- The browser is a visual interface.
- Both use the same C++ engine.
- The Python layer is a thin local bridge.
- The data files remain simple and inspectable.

The project is ready to demonstrate as a console application and as a local web application. It is also a strong base for further work in transit modelling, persistence, accessibility, and full-stack software engineering.

---

## 26. Appendices

### Appendix A: Copy-paste command reference

```powershell
# Go to the project
cd "C:\Users\Prajwal\Desktop\PuneBusPlanner"

# Build the console application
g++ -std=c++17 -O2 -Wall -Wextra -o OOP_CP.exe OOP_CP.cpp

# Run the console
.\OOP_CP.exe

# Run without ANSI colour
.\OOP_CP.exe --no-color

# Run self-tests
.\OOP_CP.exe --test

# Run the JSON API manually
.\OOP_CP.exe --api

# Run the web dashboard
.\run_ui.bat

# Or run the server directly
python server.py

# Use a different port
$env:PORT = "8765"
python server.py

# Do not open a browser automatically
$env:NO_BROWSER = "1"
python server.py
```

### Appendix B: API quick reference

| Endpoint | Method | Body fields | Response key |
|---|---|---|---|
| `/api/network` | GET | none | `network` |
| `/api/plan` | POST | `start`, `dest`, `mode`, `hour`, `passenger`, `bus` | `plan` |
| `/api/compare` | POST | `start`, `dest`, `mode`, `hour`, `passenger` | `compare` |
| `/api/book` | POST | plan fields + `name` | `ticket` |

All responses include `ok`. Errors use `{"ok": false, "error": "..."}`.

### Appendix C: Source line map

| Range | Component |
|---:|---|
| 1–21 | Includes |
| 26–31 | `trimCopy` |
| 37–131 | Presentation helpers |
| 133–168 | Enums and parsers |
| 170–213 | Exceptions |
| 215–247 | `RouteStop` |
| 249–318 | `Bus` |
| 320–383 | Concrete buses |
| 385–399 | `BusFactory` |
| 401–461 | Fare hierarchy and factory |
| 463–575 | `Passenger`, `Ticket` |
| 577–829 | Graph and Dijkstra |
| 831–845 | `RouteOptimizer<T>` |
| 847–1544 | `BusNetworkManager` |
| 1546–1573 | Input helpers |
| 1575–1710 | Self-tests |
| 1712–2086 | JSON API |
| 2088–2295 | `main` |

### Appendix D: File size summary

| File | Size |
|---|---:|
| `OOP_CP.cpp` | 90,098 bytes |
| `server.py` | 6,213 bytes |
| `web_ui/app.js` | 17,588 bytes |
| `web_ui/style.css` | 14,623 bytes |
| `web_ui/index.html` | 4,949 bytes |
| `routes.txt` | 1,828 bytes |
| `trip_log.txt` | 267 bytes |
| `run_ui.bat` | 210 bytes |

### Appendix E: Glossary

| Term | Meaning |
|---|---|
| Adjacency list | Map from a node to its outgoing roads |
| ANSI | Escape codes used for terminal colour |
| API | Application Programming Interface |
| Boarding fee | Fixed charge added to a fare |
| Dwell | Time spent at an intermediate stop |
| Edge | A road connecting two stops |
| Fare strategy | Interchangeable fare calculation algorithm |
| Fleet | Collection of bus objects |
| Graph | Nodes and weighted edges |
| GTFS | General Transit Feed Specification |
| Itinerary | Ordered list of stops and road legs |
| JSON | JavaScript Object Notation |
| Node | A stop in the graph |
| Priority queue | Data structure that returns the lowest-cost item |
| RAII | Resource Acquisition Is Initialization |
| Rush multiplier | Time-of-day slowdown factor |
| State space | Graph of `(stop, line)` states used by Dijkstra |
| Transfer | A change from one line to another |
| Undirected edge | A road usable in both directions |
| Unique pointer | `std::unique_ptr`, exclusive ownership smart pointer |

### Appendix F: Final verification checklist

- [x] Source files present in the project folder.
- [x] `OOP_CP.cpp` compiles with g++ 16.1.0 and no warnings.
- [x] `--test` prints the success message.
- [x] `routes.txt` contains 25 stops and 25 roads.
- [x] `trip_log.txt` parses into 3 historical trips.
- [x] Network API returns 25 stops, 25 roads, 4 lines, 4 buses.
- [x] Plan API returns the expected Pimpri-to-Airport result.
- [x] Compare API returns four found bus options.
- [x] Booking and cancellation work in an isolated copy.
- [x] Ticket IDs synchronize across restarts.
- [x] `server.py` starts and serves the dashboard.
- [x] Root page returns HTTP 200.
- [x] `app.js` passes `node --check`.
- [x] `server.py` passes a Python compile check.
- [x] Network, Plan, and Compare tabs render in headless Edge.
- [x] Screenshots are saved in `screenshots/`.
- [x] Run instructions are documented above.

---

**End of report.**
