# Project Brief Report
## Pune Smart Bus Route Planner (C++17)

### 1. Project Overview
The Pune Smart Bus Route Planner is a C++17 application that models a city bus network as a weighted graph. Stops are nodes, roads are edges, and multiple bus lines share transfer stops. The system finds optimal routes, calculates fares, books and cancels tickets, and maintains trip statistics.

It runs as a styled console application and as a browser dashboard. The project preserves the original OOP assignment structure while adding graph routing, design patterns, file handling, exception safety, and a local web interface.

### 2. Objectives
- Model a realistic Pune bus network using graph data structures.
- Find routes using Dijkstra's algorithm in three modes:
  - Fastest journey
  - Shortest distance
  - Fewest transfers
- Apply OOP principles, inheritance, polymorphism, templates, and design patterns.
- Provide fare calculation, seat booking, cancellation, and history tracking.
- Present results through clear tables, colour, and an itinerary view.

### 3. Key Features
- **Pune network:** 25 stops, 25 roads, 4 lines — Purple, Aqua, Red, and Green.
- **Transfer stops:** Shivajinagar, Civil_Court, Swargate, and Bund_Garden.
- **Four bus types:** Regular, Express, AC, and Electric.
- **Fare strategies:** Regular, Express, Student discount, and Senior discount.
- **Booking system:** Passenger records, seat inventory, ticket generation, cancellation.
- **Rush-hour model:** Automatic time-based fare/time multiplier.
- **Dwell time:** Added for intermediate stops.
- **Fleet comparison:** Side-by-side distance, time, transfers, fare, and strategy.
- **Trip analytics:** Revenue, distance, busiest route, recent trips, cancelled tickets.
- **Presentation layer:** ANSI colour, ASCII tables, banners, and `--no-color` fallback.
- **Browser UI:** Network map, journey planner, fleet comparison, and booking via `server.py`.
- **JSON API mode:** `--api` exposes the C++ logic to the local web UI.

### 4. Main OOP Concepts Used
| Concept | Implementation |
|---|---|
| Encapsulation | `Bus`, `RouteStop`, `Passenger`, `Ticket`, `BusNetworkManager` |
| Inheritance | `RegularBus`, `ExpressBus`, `ACBus`, `ElectricBus` derive from `Bus` |
| Runtime polymorphism | Virtual bus and fare functions |
| Abstraction | `Bus` and `FareStrategy` abstract interfaces |
| Templates | `RouteOptimizer<T>` for cumulative distance |
| Strategy pattern | `RegularFare`, `ExpressFare`, `StudentDiscount`, `SeniorDiscount` |
| Factory pattern | `BusFactory`, `FareStrategyFactory` |
| Smart pointers | `unique_ptr<Bus>` fleet ownership |
| Exception handling | Custom exceptions for booking, input, stops, routes |
| File handling | `routes.txt` loading and `trip_log.txt` persistence |
| STL containers | `vector`, `map`, `set`, `priority_queue` |
| Operator overloading | `Ticket::operator<<` |

### 5. Code Organization
| Component | Responsibility |
|---|---|
| `RouteStop` | Stores stop ID and name |
| `Bus` hierarchy | Bus capacity, fare rate, speed, seat reservation |
| `BusFactory` | Creates bus objects through `unique_ptr` |
| `FareStrategy` hierarchy | Calculates fares for passenger and bus types |
| `Passenger`, `Ticket` | Booking records and ticket display |
| `Road`, `RouteResult` | Graph edge and route result data |
| `BusGraph` | Network storage, validation, Dijkstra routing |
| `BusNetworkManager` | Fleet, booking, cancellation, statistics, file handling |
| Presentation helpers | `Style`, table rendering, `printTitle`, `printKV`, `askNumber` |
| `runSelfTests()` | Assert-based verification |
| JSON API mode | `--api` line-based transport for the web dashboard |
| `main()` | Menu loop, CLI flags, input handling, error reporting |

### 6. Data Files
- **`routes.txt`** — defines stops and undirected roads.
- **`trip_log.txt`** — stores completed bookings and `CANCEL` markers.
- **`web_ui/`** — dashboard HTML, CSS, and JavaScript.
- **`server.py`** — local HTTP bridge to the C++ `--api` backend.
- If `routes.txt` is missing, the program automatically creates the Pune network.

### 7. Build and Run
```text
g++ -std=c++17 -Wall -Wextra -o OOP_CP.exe OOP_CP.cpp
.\OOP_CP.exe
.\OOP_CP.exe --no-color
.\OOP_CP.exe --test
run_ui.bat
```

`--test` runs assert-based checks.  
`--no-color` or `NO_COLOR=1` disables ANSI colour for reports and older terminals.  
`run_ui.bat` (or `python server.py`) starts the browser dashboard at `http://127.0.0.1:8000`.

### 8. Testing and Verification
The project was compiled with:
```text
g++ -std=c++17 -Wall -Wextra
```
No compiler warnings were produced. Self-tests passed for:
- ETA calculations
- Fare strategies
- Dijkstra shortest vs fastest route selection
- Same-origin/destination handling
- Unreachable and invalid stops
- Transfer counting
- Graph validation
- Full-bus booking exception

A scripted sample booking from **Pimpri to Pune_Airport** succeeded:
- 29.20 km
- 2 transfers
- 73.7 minutes
- Rs. 123.12
- Ticket `TKT-1001`

The web layer was also verified: the server started successfully, the network
endpoint returned 25 stops and 4 lines, journey planning returned the expected
route, fleet comparison returned four options, and a booking request created a
ticket and reduced the available seat count. `server.py` and `app.js` pass
syntax checks, and all three dashboard tabs were rendered in headless Edge.

### 9. Sample Scenario
For a journey from **Pimpri** to **Pune_Airport** using the **Rapid Express Bus**:
- Route: Pimpri → Chinchwad → Khadki → Shivajinagar → Bund_Garden → Viman_Nagar → Kharadi → Pune_Airport
- Transfers: Shivajinagar and Bund_Garden
- Mode: Fastest
- Fare strategy: ExpressFare

### 10. Limitations
- Roads are treated as undirected.
- Stop names use underscores instead of spaces in `routes.txt`.
- Ticket history is append-only; active tickets exist only for the current session.
- Traffic is modelled by a simple rush-hour multiplier, not live data.
- ANSI colour depends on terminal support.
- The source is approximately 2,295 lines, so splitting into headers/sources is recommended for a larger submission.
- The browser UI requires Python 3 to run `server.py`; the console app does not.
- On systems where Device Guard blocks executables in downloaded folders, the server builds the `--api` backend in a temporary directory.

### 11. Conclusion
The project combines core OOP concepts with practical graph routing, a complete booking workflow, and a local browser dashboard. It demonstrates inheritance, polymorphism, templates, design patterns, STL usage, file handling, exception safety, and user-focused presentation in a single C++17 application.
