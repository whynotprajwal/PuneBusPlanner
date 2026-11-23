# OOP_CP.cpp Upgrade Report — Pune Smart Bus Route Planner (C++17)

## 1. What the upgraded project is
`OOP_CP.cpp` is now a graph-based Pune Smart Bus Route Planner. Stops are nodes
and bus roads are weighted edges. Multiple lines share stops, so transfers are
possible. The program supports three optimization modes, four bus types,
strategy-based fares, seat booking/cancellation, a fleet comparison table, a
presentation layer with optional ANSI colour, and trip-history analytics read
from `trip_log.txt`.

The network now contains 25 Pune stops across four lines: Purple Line, Aqua
Line, Red Line, and Green Line. The original classes and menu flow are
preserved: `RouteStop`, `Bus`, `RegularBus`, `ExpressBus`, `RouteOptimizer<T>`,
`BusNetworkManager`, the custom exceptions, the console menu, and the
`UNIT 1–5` annotations.

## 2. Change list and UNIT/OOP mapping

| Change | UNIT / OOP concept demonstrated |
|---|---|
| Stops as graph nodes, roads as weighted edges | UNIT 7: graph modelling, STL `map`/`vector`, composition |
| Multiple lines sharing transfer stops | UNIT 7: graph connectivity, transfer semantics |
| Dijkstra for fastest / shortest / fewest transfers | UNIT 7: Dijkstra, `priority_queue`, state-space search |
| `BusGraph` class | UNIT 1/7: encapsulation + graph abstraction |
| `OptimizationMode`, `PassengerType`, `BusKind` | UNIT 6: `enum class` type safety |
| `RegularFare`, `ExpressFare`, `StudentDiscount`, `SeniorDiscount` | UNIT 6: Strategy pattern; UNIT 3: runtime polymorphism |
| `FareStrategyFactory` | UNIT 6: Factory pattern |
| `ACBus`, `ElectricBus` | UNIT 3: hierarchical inheritance + overridden virtual functions |
| `BusFactory` returning `unique_ptr<Bus>` | UNIT 6: Factory pattern; UNIT 8: RAII/smart pointers |
| `Passenger`, `Ticket`, seat inventory, cancellation | UNIT 1: classes/encapsulation; UNIT 4: domain exceptions |
| `BusFullException` | UNIT 4: custom exception hierarchy |
| `InvalidInputException` | UNIT 4: separate input exception (no longer reusing `InvalidStopException`) |
| Dwell per intermediate stop | UNIT 2/7: time model in weighted routing |
| Time-of-day rush-hour multiplier | UNIT 7: dynamic edge costs; replaces manual congestion input |
| Compare-all-bus-types table | UNIT 3/7: polymorphism + formatted table output |
| Trip history and statistics from `trip_log.txt` | UNIT 5: file I/O; STL `map`/`set`/`vector` |
| Ticket ID sync across restarts | UNIT 5: file reading + static state |
| `Bus` non-copyable/non-movable (Rule of Five) | UNIT 3/8: special member functions; fixes static-counter fragility |
| Strings passed by `const&` | UNIT 1: const-correctness and efficiency |
| `operator<<` builds ticket text without changing stream flags | UNIT 2: operator overloading done safely |
| Validated `cin` reads with `clear()`/`ignore()` | UNIT 5/8: stream-state handling, `<limits>` |
| Reusable validated prompt helper `askNumber()` | UNIT 5/8: checked input, less duplication |
| `routes.txt` validation | UNIT 5: file parsing, malformed-data handling |
| ANSI colour + ASCII box/table helpers | UNIT 2: formatted output; UNIT 8: usability |
| `--no-color` and `NO_COLOR` fallback | UNIT 8: portability across terminals and CI logs |
| Pune 25-stop / 4-line seed network | UNIT 5/7: realistic graph data + automatic file seeding |
| Assert-based self-tests | UNIT 4/7: testing ETA, fare, Dijkstra, booking edge cases |

## 3. Fixed issues from the original version

1. `BusNetworkManager` is constructed **inside** `main()`'s `try`.
2. `InvalidInputException` is now separate from `InvalidStopException`.
3. Every numeric `cin` read is checked; failures use `clear()` and
   `ignore(numeric_limits<streamsize>::max(), '\n')`.
4. `routes.txt` validation covers: unknown directives, malformed token counts,
   non-numeric values, negative/zero distances, invalid speeds, duplicate stop
   IDs, self-loops, and edges referring to unknown stops.
5. `Bus` is non-copyable and non-movable, so the static live-bus counter cannot
   be corrupted by copies. Strings are passed by `const&`.
6. `operator<<` no longer depends on `setw`/`left` side effects; it builds the
   ticket text without changing global stream formatting.
7. No manual `new`/`delete`: `BusFactory` returns `unique_ptr<Bus>`, and the
   manager owns the fleet.
8. Cancelled bookings are marked in the log and excluded from revenue/statistics.
9. Presentation helpers keep the UI consistent and can be disabled with
   `--no-color` or the `NO_COLOR` environment variable.
10. The seed network and `routes.txt` now use the Pune 25-stop, 4-line network
    instead of the old eight-stop sample.
11. Numeric prompts use the shared `askNumber()` helper, so validation and
    prompt styling stay consistent across the menu.

## 4. Build and run

```text
g++ -std=c++17 -Wall -Wextra -o OOP_CP.exe OOP_CP.cpp
.\OOP_CP.exe
.\OOP_CP.exe --no-color
.\OOP_CP.exe --test
```

Colour is enabled by default. Use `--no-color` or set `NO_COLOR=1` when the
output is redirected, pasted into a report, or viewed in a terminal that does
not understand ANSI escape sequences. Plain ASCII output remains fully readable.

The self-test mode runs assert-based checks for ETA, fares, Dijkstra edge cases
(same start/end, unreachable stop, invalid ID, shortest-vs-fastest choice),
transfer counting, graph validation, and `BusFullException`.

### Web UI

The browser dashboard reuses the same C++ routing, fare, fleet, and booking
logic; it does not reimplement any planner rules in JavaScript.

```text
run_ui.bat
```

or:

```text
python server.py
```

The launcher starts `server.py`, which builds `OOP_CP.cpp` into a temporary
backend executable and keeps one `--api` process alive. It then opens
`http://127.0.0.1:8000`. Use `PORT=8765` to change the port and
`NO_BROWSER=1` to suppress the browser launch (useful for tests). The UI files
are in `web_ui/` (`index.html`, `style.css`, `app.js`).

For direct integration, the C++ backend can be run manually:

```text
.\OOP_CP.exe --api
```

It reads one JSON request per line and writes one JSON response per line with
actions `network`, `plan`, `compare`, and `book`. The API mode is a transport
layer only; all calculations still run through the existing C++ classes.

## 5. Sample routes.txt (Pune network)

```text
# Pune Smart Bus Route Planner - network definition
# STOP <id> <name>
# EDGE <fromId> <toId> <distanceKm> <speedLimitKmh> <lineName>
# STOP lines must appear before EDGE lines.
STOP 1 Pimpri
STOP 2 Chinchwad
STOP 3 Khadki
STOP 4 Shivajinagar
STOP 5 Civil_Court
STOP 6 Swargate
STOP 7 Vanaz
STOP 8 Kothrud
STOP 9 Deccan_Gymkhana
STOP 10 PMC
STOP 11 Pune_Station
STOP 12 Kalyani_Nagar
STOP 13 Ramwadi
STOP 14 Katraj
STOP 15 Camp
STOP 16 Bund_Garden
STOP 17 Viman_Nagar
STOP 18 Kharadi
STOP 19 Pune_Airport
STOP 20 Hinjawadi
STOP 21 Wakad
STOP 22 Baner
STOP 23 Aundh
STOP 24 Hadapsar
STOP 25 Magarpatta

# Purple Line
EDGE 1 2 3.0 40 PurpleLine
EDGE 2 3 4.5 35 PurpleLine
EDGE 3 4 5.0 35 PurpleLine
EDGE 4 5 2.5 30 PurpleLine
EDGE 5 6 2.0 30 PurpleLine

# Aqua Line
EDGE 7 8 4.0 35 AquaLine
EDGE 8 9 3.5 35 AquaLine
EDGE 9 10 2.2 30 AquaLine
EDGE 10 5 1.8 30 AquaLine
EDGE 5 11 2.5 30 AquaLine
EDGE 11 12 4.0 40 AquaLine
EDGE 12 13 5.0 45 AquaLine

# Red Line
EDGE 14 6 6.5 35 RedLine
EDGE 6 15 2.2 30 RedLine
EDGE 15 16 2.0 30 RedLine
EDGE 16 17 3.2 35 RedLine
EDGE 17 18 4.5 45 RedLine
EDGE 18 19 4.0 50 RedLine

# Green Line
EDGE 20 21 4.0 40 GreenLine
EDGE 21 22 3.5 35 GreenLine
EDGE 22 23 3.0 35 GreenLine
EDGE 23 4 5.5 40 GreenLine
EDGE 4 16 5.0 40 GreenLine
EDGE 16 24 6.0 45 GreenLine
EDGE 24 25 3.0 40 GreenLine
```

If `routes.txt` is missing, the program writes this same Pune network
automatically.

## 6. Sample run (condensed, `--no-color`)

```text
================================================================
                    SMART BUS ROUTE PLANNER
================================================================
  Pune City Network  |  Dijkstra Routing  |  OOP Demo
----------------------------------------------------------------
   [1]  View Route Network & Fleet
   [2]  Plan Journey (Dijkstra + Fare + Booking)
   [3]  Compare All Bus Types
   [4]  Cancel Ticket
   [5]  Trip History & Statistics
   [6]  System Statistics
   [7]  Exit System
----------------------------------------------------------------

  Enter your choice: 2

================================================================
                        PUNE BUS NETWORK
================================================================

  STOPS (25)
  [ 1] Pimpri                 [ 2] Chinchwad
  [ 3] Khadki                 [ 4] Shivajinagar
  ...
  [23] Aundh                  [24] Hadapsar
  [25] Magarpatta

  LINES & ROADS
  * AquaLine
      Vanaz              <-> Kothrud             4.0 km (35 km/h)
      Kothrud            <-> Deccan_Gymkhana     3.5 km (35 km/h)
      ...
  * PurpleLine
      Pimpri             <-> Chinchwad           3.0 km (40 km/h)
      ...

  AVAILABLE FLEET
+-----+------------------------+-----------+-----------+-----------+
|   # | Bus Type               | Bus ID    |     Seats |     Rs/km |
+-----+------------------------+-----------+-----------+-----------+
|   1 | Regular Feeder Bus     | REG-101   |      0/60 |      2.00 |
|   2 | Rapid Express Bus      | EXP-202   |      0/45 |      3.00 |
|   3 | Comfort AC Bus         | AC-303   |      0/40 |      3.20 |
|   4 | Eco Electric Bus       | ELE-404   |      0/50 |      1.80 |
+-----+------------------------+-----------+-----------+-----------+

  Enter Origin Stop ID: 1
  Enter Destination Stop ID: 19
  Mode (1 Fastest, 2 Shortest, 3 Fewest Transfers): 1
  Time of Day (0-23, rush applied automatically): 9
  Passenger Type (1 Regular, 2 Student, 3 Senior): 1
  Fleet Type (1-4): 2

================================================================
                        JOURNEY SUMMARY
================================================================
  Optimization Mode        : Fastest
  Origin                   : Pimpri
  Destination              : Pune_Airport
  Bus Allocated            : Rapid Express Bus [EXP-202]
  Intermediate Stops       : 6
  Transfers                : 2
  Corridor Distance        : 29.20 km
  Time of Day              : 09:00  [RUSH x1.80]
  Estimated Time           : 73.7 minutes (includes dwell)
  Fare Strategy            : ExpressFare
  Estimated Fare           : Rs. 123.12
  Seat Availability        : 45/45

  ROUTE ITINERARY
  ---------------
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
----------------------------------------------------------------

  Book a seat? (1 = Yes, 0 = No): 1
  Passenger name: Prajwal
  [Log saved to trip_log.txt]

  BOOKING CONFIRMED

================================================================
                        TICKET TKT-1001
================================================================
  Passenger              : Prajwal (Regular)
  Bus                    : Rapid Express Bus [EXP-202]
  Route                  : Pimpri -> Pune_Airport
  Distance               : 29.20 km
  ETA                    : 73.7 minutes
  Fare                   : Rs. 123.12 (ExpressFare)
----------------------------------------------------------------

================================================================
                        FLEET COMPARISON
================================================================
  Optimization Mode        : Fastest
  Time of Day              : 09:00  [rush x1.80]
  Passenger Type           : Regular

  OPTIONS
  -------
+------------------------+------------+------------+-----------+-----------+--------------------+
| Bus Type               |   Distance |       Time | Transfers |      Fare | Strategy           |
+------------------------+------------+------------+-----------+-----------+--------------------+
| Regular Feeder Bus     |    29.2 km |  126.1 min |         2 |     68.40 | RegularFare        |
| Rapid Express Bus      |    29.2 km |   73.7 min |         2 |    123.12 | ExpressFare        |
| Comfort AC Bus         |    29.2 km |   84.2 min |         2 |    111.44 | RegularFare        |
| Eco Electric Bus       |    29.2 km |   77.9 min |         2 |     60.56 | RegularFare        |
+------------------------+------------+------------+-----------+-----------+--------------------+

================================================================
                      TRIP HISTORY & STATS
================================================================
  Total Trips              : 1
  Total Revenue            : Rs. 123.12
  Busiest Route            : Pimpri -> Pune_Airport (1 trip)
  Total Distance           : 29.2 km
  Cancelled Tickets        : 0

  RECENT TRIPS
  ------------
+-------------+----------------------------------------+------------------------+------------+------------+
| Ticket      | Route                                  | Bus                    |        ETA |       Fare |
+-------------+----------------------------------------+------------------------+------------+------------+
| TKT-1001    | Pimpri -> Pune_Airport                 | Rapid Express Bus      |   73.7 min | Rs. 123.12 |
+-------------+----------------------------------------+------------------------+------------+------------+
```

## 7. Suggested viva questions and short answers

**1. Why use Dijkstra instead of BFS/DFS?**  
Roads have non-negative weights (distance/time), and Dijkstra finds the
minimum-cost path. BFS only guarantees the shortest path in unweighted graphs;
DFS does not guarantee optimality at all.

**2. How does the Dijkstra state track transfers?**  
The state is `(stop, currentLine)`. When the next road belongs to a different
line, the transfer cost increases by one. Keeping the line in the state is
necessary; otherwise the algorithm cannot know whether a line change occurred.
For "fewest transfers", the primary cost is transfers and distance is the
tie-breaker.

**3. What is the Strategy pattern, and where is it used?**  
It defines a family of interchangeable fare algorithms behind a common
`FareStrategy` interface: `RegularFare`, `ExpressFare`, `StudentDiscount`, and
`SeniorDiscount`. `FareStrategyFactory` selects the right strategy at runtime,
so new fare rules do not require changes to booking code.

**4. Why is `Bus` non-copyable, and how does that help the static counter?**  
A copied bus would not run the constructor logic, but its destructor would still
decrement `totalBusesActive`, corrupting the count. Deleting copy/move (Rule of
Five) ensures only factory-created buses exist, so construction/destruction and
the counter stay balanced. `unique_ptr` then provides automatic cleanup.

**5. How is dwell time modeled, and why is one dwell removed in fastest mode?**  
Each intermediate stop adds one dwell interval. In fastest routing, each edge
includes one dwell, so a path with `E` edges has `E` dwells. The destination is
not an intermediate stop, so the final code subtracts one dwell after
reconstructing the path. Displayed ETA therefore uses `E - 1` dwells.

**6. How is the presentation layer kept optional and portable?**  
All colour and table helpers live in a small `Style`/presentation block. The
`Style::paint()` function returns plain text when colour is disabled. Colour is
turned off by `--no-color` or `NO_COLOR=1`, so redirected output, reports, and
older terminals still receive readable ASCII tables.

## 8. File size and suggested header/source split

The upgraded single file is about 2,295 lines, so a split is reasonable for a
larger submission (and preferable to exceeding the ~900-line guideline):

- `exceptions.hpp` — custom exception classes
- `models.hpp/.cpp` — `RouteStop`, `Bus` hierarchy, `Passenger`, `Ticket`
- `fares.hpp/.cpp` — `FareContext`, `FareStrategy` family, fare factory
- `graph.hpp/.cpp` — `Road`, `RouteResult`, `BusGraph`, Dijkstra
- `bus_factory.hpp/.cpp` — `BusFactory`
- `ui.hpp/.cpp` — `Style`, colour handling, ASCII table/box helpers
- `manager.hpp/.cpp` — `BusNetworkManager`
- `api.hpp/.cpp` — JSON transport for `--api` mode
- `main.cpp` — menu, CLI flags, prompts, and input helpers
- `tests.cpp` — assert-based self-tests

The web layer is already separated: `server.py` is a small stdlib HTTP bridge,
and `web_ui/` contains the dashboard HTML, CSS, and JavaScript.

## 9. Notes and limitations

- The Pune network has 25 stops, 25 roads, and 4 lines. Shared transfer stops
  are Shivajinagar, Civil_Court, Swargate, and Bund_Garden.
- `trip_log.txt` is append-only. Bookings are excluded from statistics when a
  matching `CANCEL` line exists.
- Active tickets are held in memory for the current session; trip history
  persists across runs, and ticket IDs are synchronized from the log so they do
  not repeat.
- `routes.txt` must define all `STOP` lines before `EDGE` lines.
- Roads are treated as undirected (both directions are added automatically).
- Stop names cannot contain spaces; use underscores.
- Colour is optional. Use `--no-color` or `NO_COLOR=1` for plain ASCII output;
  ANSI colour works best in Windows Terminal or a modern conhost window.
- The browser UI needs Python 3 for `server.py`; the C++ console app has no
  runtime dependency beyond the compiled executable.
- On systems with Device Guard blocking executables in downloaded folders,
  build the `--api` backend in a temporary directory (`server.py` does this
  automatically).
- No external C++ libraries; plain C++17 with the standard library only.
