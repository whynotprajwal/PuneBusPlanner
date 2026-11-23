#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iomanip>
#include <exception>
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <limits>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <utility>

using namespace std;

// ==========================================
// SMALL UTILITIES
// ==========================================
static string trimCopy(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// ==========================================
// [NEW] UNIT 8: CONSOLE PRESENTATION HELPERS
// (ANSI colour is optional; use --no-color to disable)
// ==========================================
namespace Style {
    bool enabled = true;

    const string RESET   = "\033[0m";
    const string BOLD    = "\033[1m";
    const string DIM     = "\033[2m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[97m";

    string paint(const string& text, const string& color) {
        if (!enabled || color.empty()) return text;
        return color + text + RESET;
    }
}

static string fitRight(const string& text, size_t width) {
    if (text.size() <= width) return text + string(width - text.size(), ' ');
    if (width <= 3) return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

static string fitLeft(const string& text, size_t width) {
    if (text.size() <= width) return string(width - text.size(), ' ') + text;
    if (width <= 3) return text.substr(0, width);
    return text.substr(0, width - 3) + "...";
}

static string numberString(float value, int precision) {
    ostringstream oss;
    oss << fixed << setprecision(precision) << value;
    return oss.str();
}

static string lineColor(const string& line) {
    if (line.find("Purple") != string::npos) return Style::MAGENTA;
    if (line.find("Aqua") != string::npos) return Style::CYAN;
    if (line.find("Red") != string::npos) return Style::RED;
    if (line.find("Green") != string::npos) return Style::GREEN;
    return Style::BLUE;
}

static void printTitle(const string& title) {
    const int width = 64;
    cout << "\n" << Style::paint(string(width, '='), Style::CYAN) << "\n";
    int padding = (width - static_cast<int>(title.size())) / 2;
    if (padding < 0) padding = 0;
    cout << Style::paint(string(padding, ' ') + title, Style::BOLD + Style::CYAN) << "\n";
    cout << Style::paint(string(width, '='), Style::CYAN) << "\n";
}

static void printSubtitle(const string& title) {
    cout << "\n  " << Style::paint(title, Style::BOLD + Style::YELLOW) << "\n";
    cout << "  " << Style::paint(string(title.size(), '-'), Style::DIM) << "\n";
}

static void printRule(size_t width = 64, char fill = '-') {
    cout << Style::paint(string(width, fill), Style::DIM) << "\n";
}

static void printKV(const string& label, const string& value, int labelWidth = 24) {
    cout << "  " << Style::paint(fitRight(label, labelWidth), Style::DIM)
         << " : " << value << "\n";
}

static void printTableSeparator(const vector<int>& widths, char fill = '-') {
    cout << Style::paint("+", Style::DIM);
    for (int width : widths) {
        cout << Style::paint(string(width + 2, fill) + "+", Style::DIM);
    }
    cout << "\n";
}

static void printTableRow(const vector<string>& cells,
                          const vector<int>& widths,
                          const vector<string>& colors = {}) {
    cout << Style::paint("|", Style::DIM);
    for (size_t i = 0; i < cells.size(); ++i) {
        string cell = fitRight(cells[i], static_cast<size_t>(widths[i]));
        if (i < colors.size() && !colors[i].empty()) {
            cell = Style::paint(cell, colors[i]);
        }
        cout << " " << cell << " " << Style::paint("|", Style::DIM);
    }
    cout << "\n";
}

static void printMenuItem(int number, const string& text) {
    cout << "   " << Style::paint("[" + to_string(number) + "]", Style::CYAN)
         << "  " << text << "\n";
}

// [NEW] UNIT 6: ENUM CLASSES (type-safe small value sets)
enum class OptimizationMode { FASTEST = 1, SHORTEST = 2, FEWEST_TRANSFERS = 3 };
enum class PassengerType { REGULAR = 1, STUDENT = 2, SENIOR = 3 };
enum class BusKind { REGULAR = 1, EXPRESS = 2, AC = 3, ELECTRIC = 4 };

static string optimizationModeToString(OptimizationMode mode) {
    switch (mode) {
        case OptimizationMode::FASTEST: return "Fastest";
        case OptimizationMode::SHORTEST: return "Shortest Distance";
        case OptimizationMode::FEWEST_TRANSFERS: return "Fewest Transfers";
    }
    return "Unknown";
}

static string passengerTypeToString(PassengerType type) {
    switch (type) {
        case PassengerType::REGULAR: return "Regular";
        case PassengerType::STUDENT: return "Student";
        case PassengerType::SENIOR: return "Senior";
    }
    return "Regular";
}

static OptimizationMode parseOptimizationMode(int value) {
    if (value == 1) return OptimizationMode::FASTEST;
    if (value == 2) return OptimizationMode::SHORTEST;
    if (value == 3) return OptimizationMode::FEWEST_TRANSFERS;
    throw std::invalid_argument("mode");
}

static PassengerType parsePassengerType(int value) {
    if (value == 1) return PassengerType::REGULAR;
    if (value == 2) return PassengerType::STUDENT;
    if (value == 3) return PassengerType::SENIOR;
    throw std::invalid_argument("passenger");
}

// ==========================================
// UNIT 4: CUSTOM EXCEPTION CLASSES
// ==========================================
class RouteNotFoundException : public exception {
private:
    string message;
public:
    RouteNotFoundException(const string& msg) : message("Route Error: " + msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class InvalidStopException : public exception {
private:
    string message;
public:
    InvalidStopException(const string& msg) : message("Input Error: " + msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// [NEW] UNIT 4: SEPARATE INPUT EXCEPTION (no longer reusing InvalidStopException)
class InvalidInputException : public exception {
private:
    string message;
public:
    InvalidInputException(const string& msg) : message("Invalid Input: " + msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// [NEW] UNIT 4: DOMAIN-SPECIFIC EXCEPTION FOR SEAT BOOKING
class BusFullException : public exception {
private:
    string message;
public:
    BusFullException(const string& msg) : message("Booking Error: " + msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// ==========================================
// UNIT 1 & 2: OOP BASICS, CONSTRUCTORS, OVERLOADING
// ==========================================
class RouteStop {
private:
    int stopId;
    string stopName;
    float distanceFromPrevious; // kept for backwards compatibility

public:
    RouteStop() : stopId(0), stopName("Unknown"), distanceFromPrevious(0.0f) {}

    RouteStop(int id, const string& name, float dist) {
        this->stopId = id;          // 'this' pointer
        this->stopName = name;
        this->distanceFromPrevious = dist;
    }

    int getId() const { return stopId; }
    const string& getName() const { return stopName; }
    float getDistance() const { return distanceFromPrevious; }

    // [FIXED] Operator Overloading restores stream flags after setw/left
    friend ostream& operator<<(ostream& os, const RouteStop& stop);
};

ostream& operator<<(ostream& os, const RouteStop& stop) {
    ios::fmtflags oldFlags = os.flags();
    os << "[" << stop.stopId << "] " << left << setw(22) << stop.stopName;
    os.flags(oldFlags);
    return os;
}

// ==========================================
// UNIT 3: INHERITANCE & RUNTIME POLYMORPHISM
// (Abstract Class, Pure Virtual Functions)
// ==========================================
class Bus {
protected:
    string busId;
    string busType;
    int seatCapacity;
    int seatsBooked;
    float baseSpeed;        // km/h
    float dwellMinutes;     // minutes added per intermediate stop
    float fareRatePerKm;
    float boardingFee;
    static int totalBusesActive; // [FIXED] safe because Bus is non-copyable

public:
    Bus(const string& id, const string& type, int capacity, float speed,
        float dwell, float ratePerKm, float boarding)
        : busId(id), busType(type), seatCapacity(capacity), seatsBooked(0),
          baseSpeed(speed), dwellMinutes(dwell), fareRatePerKm(ratePerKm),
          boardingFee(boarding) {
        if (capacity <= 0 || speed <= 0.0f || dwell < 0.0f) {
            throw InvalidInputException("Invalid configuration for bus " + id);
        }
        totalBusesActive++;
    }

    // [FIXED] Rule of Five: non-copyable and non-movable (prevents counter corruption)
    Bus(const Bus&) = delete;
    Bus& operator=(const Bus&) = delete;
    Bus(Bus&&) = delete;
    Bus& operator=(Bus&&) = delete;

    virtual ~Bus() {
        totalBusesActive--;
    }

    // [FIXED] signature now takes road speed limit and rush multiplier
    virtual float calculateTripTime(float distance, float roadSpeedLimit,
                                    float rushMultiplier) const = 0;

    virtual bool isExpressBus() const { return false; }

    const string& getId() const { return busId; }
    const string& getType() const { return busType; }
    int getCapacity() const { return seatCapacity; }
    int getSeatsBooked() const { return seatsBooked; }
    int getAvailableSeats() const { return seatCapacity - seatsBooked; }
    float getBaseSpeed() const { return baseSpeed; }
    float getDwellMinutes() const { return dwellMinutes; }
    float getFareRate() const { return fareRatePerKm; }
    float getBoardingFee() const { return boardingFee; }

    void reserveSeat() {
        if (seatsBooked >= seatCapacity) {
            throw BusFullException("Bus " + busId + " is full (" +
                                   to_string(seatsBooked) + "/" +
                                   to_string(seatCapacity) + " seats).");
        }
        ++seatsBooked;
    }

    void releaseSeat() {
        if (seatsBooked > 0) --seatsBooked;
    }

    static int getActiveBusCount() { return totalBusesActive; }
};

int Bus::totalBusesActive = 0;

// Helper shared by concrete buses
static float effectiveSpeedKmh(float baseSpeed, float roadLimit, float rushAdjusted) {
    float speed = std::min(baseSpeed, roadLimit) / rushAdjusted;
    if (speed < 5.0f) speed = 5.0f;
    return speed;
}

// Hierarchical Inheritance: Regular Bus
class RegularBus : public Bus {
public:
    RegularBus(const string& id)
        : Bus(id, "Regular Feeder Bus", 60, 30.0f, 3.5f, 2.0f, 10.0f) {}

    float calculateTripTime(float distance, float roadSpeedLimit,
                            float rushMultiplier) const override {
        float rush = std::max(0.5f, rushMultiplier);
        float speed = effectiveSpeedKmh(baseSpeed, roadSpeedLimit, rush);
        return (distance / speed) * 60.0f;
    }
};

// Hierarchical Inheritance: Express Bus (reduced traffic penalty)
class ExpressBus : public Bus {
public:
    ExpressBus(const string& id)
        : Bus(id, "Rapid Express Bus", 45, 55.0f, 1.0f, 3.0f, 15.0f) {}

    bool isExpressBus() const override { return true; }

    float calculateTripTime(float distance, float roadSpeedLimit,
                            float rushMultiplier) const override {
        float adjusted = 1.0f + (std::max(0.5f, rushMultiplier) - 1.0f) * 0.65f;
        float speed = effectiveSpeedKmh(baseSpeed, roadSpeedLimit, adjusted);
        return (distance / speed) * 60.0f;
    }
};

// [NEW] UNIT 3: AC BUS SUBTYPE
class ACBus : public Bus {
public:
    ACBus(const string& id)
        : Bus(id, "Comfort AC Bus", 40, 45.0f, 2.0f, 3.2f, 18.0f) {}

    float calculateTripTime(float distance, float roadSpeedLimit,
                            float rushMultiplier) const override {
        float adjusted = 1.0f + (std::max(0.5f, rushMultiplier) - 1.0f) * 0.75f;
        float speed = effectiveSpeedKmh(baseSpeed, roadSpeedLimit, adjusted);
        return (distance / speed) * 60.0f;
    }
};

// [NEW] UNIT 3: ELECTRIC BUS SUBTYPE
class ElectricBus : public Bus {
public:
    ElectricBus(const string& id)
        : Bus(id, "Eco Electric Bus", 50, 40.0f, 1.5f, 1.8f, 8.0f) {}

    float calculateTripTime(float distance, float roadSpeedLimit,
                            float rushMultiplier) const override {
        float adjusted = 1.0f + (std::max(0.5f, rushMultiplier) - 1.0f) * 0.60f;
        float speed = effectiveSpeedKmh(baseSpeed, roadSpeedLimit, adjusted);
        return (distance / speed) * 60.0f;
    }
};

// ==========================================
// [NEW] UNIT 6: FACTORY PATTERN + SMART POINTERS
// ==========================================
class BusFactory {
public:
    static unique_ptr<Bus> create(BusKind kind) {
        switch (kind) {
            case BusKind::REGULAR:  return make_unique<RegularBus>("REG-101");
            case BusKind::EXPRESS:  return make_unique<ExpressBus>("EXP-202");
            case BusKind::AC:       return make_unique<ACBus>("AC-303");
            case BusKind::ELECTRIC: return make_unique<ElectricBus>("ELE-404");
        }
        throw InvalidInputException("Unknown bus category.");
    }
};

// ==========================================
// [NEW] UNIT 6: STRATEGY PATTERN FOR FARES
// ==========================================
struct FareContext {
    float distanceKm;
    float ratePerKm;
    float boardingFee;
};

class FareStrategy {
public:
    virtual ~FareStrategy() = default;
    virtual string name() const = 0;
    virtual float calculate(const FareContext& context) const = 0;
};

class RegularFare : public FareStrategy {
public:
    string name() const override { return "RegularFare"; }
    float calculate(const FareContext& c) const override {
        return c.boardingFee + (c.distanceKm * c.ratePerKm);
    }
};

class ExpressFare : public FareStrategy {
public:
    string name() const override { return "ExpressFare"; }
    float calculate(const FareContext& c) const override {
        return (c.boardingFee + (c.distanceKm * c.ratePerKm)) * 1.20f;
    }
};

class StudentDiscount : public FareStrategy {
public:
    string name() const override { return "StudentDiscount"; }
    float calculate(const FareContext& c) const override {
        return (c.boardingFee + (c.distanceKm * c.ratePerKm)) * 0.50f;
    }
};

class SeniorDiscount : public FareStrategy {
public:
    string name() const override { return "SeniorDiscount"; }
    float calculate(const FareContext& c) const override {
        return (c.boardingFee + (c.distanceKm * c.ratePerKm)) * 0.70f;
    }
};

class FareStrategyFactory {
public:
    static unique_ptr<FareStrategy> create(PassengerType type, bool expressBus) {
        switch (type) {
            case PassengerType::STUDENT: return make_unique<StudentDiscount>();
            case PassengerType::SENIOR:  return make_unique<SeniorDiscount>();
            case PassengerType::REGULAR:
                if (expressBus) return make_unique<ExpressFare>();
                return make_unique<RegularFare>();
        }
        return make_unique<RegularFare>();
    }
};

// ==========================================
// [NEW] UNIT 1: PASSENGER AND TICKET CLASSES
// ==========================================
class Passenger {
private:
    int passengerId;
    string passengerName;
    PassengerType category;

public:
    Passenger(int id, const string& name, PassengerType type)
        : passengerId(id), passengerName(name), category(type) {}

    int getId() const { return passengerId; }
    const string& getName() const { return passengerName; }
    PassengerType getType() const { return category; }
};

class Ticket {
private:
    static int nextSerial;
    string ticketId;
    Passenger passenger;
    string busId;
    string busType;
    string originName;
    string destinationName;
    float distanceKm;
    float minutes;
    float fare;
    string fareStrategyName;

public:
    Ticket(const Passenger& p, const string& busIdValue, const string& busTypeValue,
           const string& origin, const string& destination, float dist,
           float mins, float fareValue, const string& strategyName)
        : ticketId("TKT-" + to_string(++nextSerial)), passenger(p),
          busId(busIdValue), busType(busTypeValue), originName(origin),
          destinationName(destination), distanceKm(dist), minutes(mins),
          fare(fareValue), fareStrategyName(strategyName) {}

    const string& getId() const { return ticketId; }
    const string& getBusId() const { return busId; }
    const Passenger& getPassenger() const { return passenger; }
    float getFare() const { return fare; }

    string toLogLine() const {
        ostringstream oss;
        oss << ticketId << '\t'
            << passenger.getName() << '\t'
            << passengerTypeToString(passenger.getType()) << '\t'
            << originName << '\t'
            << destinationName << '\t'
            << busType << '\t'
            << fixed << setprecision(2) << distanceKm << '\t'
            << fixed << setprecision(1) << minutes << '\t'
            << fixed << setprecision(2) << fare << '\t'
            << fareStrategyName;
        return oss.str();
    }

    // [NEW] Keep ticket IDs unique across program restarts by inspecting the log
    static void syncSerialFromLog(const string& filename) {
        ifstream infile(filename);
        if (!infile.is_open()) return;
        string line;
        int maxSerial = nextSerial;
        while (getline(infile, line)) {
            if (line.compare(0, 4, "TKT-") != 0) continue;
            size_t start = 4;
            size_t end = line.find_first_of("\t \r", start);
            string token = line.substr(
                start, end == string::npos ? string::npos : end - start);
            try {
                int value = stoi(token);
                if (value > maxSerial) maxSerial = value;
            } catch (...) {
                // ignore malformed ticket IDs
            }
        }
        nextSerial = maxSerial;
    }

    friend ostream& operator<<(ostream& os, const Ticket& ticket);
};

int Ticket::nextSerial = 1000;

ostream& operator<<(ostream& os, const Ticket& ticket) {
    const int width = 64;
    string title = "TICKET " + ticket.ticketId;
    int padding = (width - static_cast<int>(title.size())) / 2;
    if (padding < 0) padding = 0;

    os << "\n" << Style::paint(string(width, '='), Style::CYAN) << "\n"
       << Style::paint(string(padding, ' ') + title, Style::BOLD + Style::CYAN) << "\n"
       << Style::paint(string(width, '='), Style::CYAN) << "\n";

    auto kv = [&os](const string& label, const string& value) {
        os << "  " << Style::paint(fitRight(label, 22), Style::DIM)
           << " : " << value << "\n";
    };
    kv("Passenger", ticket.passenger.getName() + " (" +
                    passengerTypeToString(ticket.passenger.getType()) + ")");
    kv("Bus", ticket.busType + " [" + ticket.busId + "]");
    kv("Route", ticket.originName + " -> " + ticket.destinationName);
    kv("Distance", numberString(ticket.distanceKm, 2) + " km");
    kv("ETA", numberString(ticket.minutes, 1) + " minutes");
    kv("Fare", Style::paint("Rs. " + numberString(ticket.fare, 2), Style::GREEN) +
                " (" + ticket.fareStrategyName + ")");
    os << Style::paint(string(width, '-'), Style::DIM) << "\n";
    return os;
}

// ==========================================
// [NEW] UNIT 7: GRAPH + DIJKSTRA
// ==========================================
struct Road {
    int fromId = 0;
    int toId = 0;
    float distanceKm = 0.0f;
    float speedLimitKmh = 0.0f;
    string lineName;
};

struct RouteResult {
    bool found = false;
    vector<int> stopIds;
    vector<Road> legs;
    int transfers = 0;
    float totalDistanceKm = 0.0f;
    float totalMinutes = 0.0f;
    string message;
};

class BusGraph {
private:
    map<int, RouteStop> stops;
    map<int, vector<Road>> adjacency;
    size_t roadCountValue = 0;

public:
    void addStop(int id, const string& name) {
        if (id <= 0) throw InvalidInputException("Stop ID must be positive.");
        if (name.empty()) throw InvalidInputException("Stop name cannot be empty.");
        if (stops.count(id) != 0) {
            throw InvalidInputException("Duplicate stop ID: " + to_string(id));
        }
        stops.emplace(id, RouteStop(id, name, 0.0f));
        adjacency[id];
    }

    void addRoad(int fromId, int toId, float distanceKm, float speedLimitKmh,
                 const string& lineName) {
        if (stops.count(fromId) == 0 || stops.count(toId) == 0) {
            throw InvalidInputException("Road references an unknown stop.");
        }
        if (fromId == toId) {
            throw InvalidInputException("Road cannot be a self-loop.");
        }
        if (distanceKm <= 0.0f || !std::isfinite(distanceKm)) {
            throw InvalidInputException("Road distance must be positive.");
        }
        if (speedLimitKmh <= 0.0f || speedLimitKmh > 200.0f ||
            !std::isfinite(speedLimitKmh)) {
            throw InvalidInputException("Invalid road speed limit.");
        }
        if (lineName.empty()) {
            throw InvalidInputException("Road line name cannot be empty.");
        }

        adjacency[fromId].push_back(Road{fromId, toId, distanceKm, speedLimitKmh, lineName});
        adjacency[toId].push_back(Road{toId, fromId, distanceKm, speedLimitKmh, lineName});
        ++roadCountValue;
    }

    bool hasStop(int id) const { return stops.count(id) != 0; }

    const RouteStop& getStop(int id) const {
        auto it = stops.find(id);
        if (it == stops.end()) {
            throw InvalidStopException("Stop ID " + to_string(id) + " does not exist.");
        }
        return it->second;
    }

    const map<int, RouteStop>& getStops() const { return stops; }
    const map<int, vector<Road>>& getAdjacency() const { return adjacency; }

    size_t stopCount() const { return stops.size(); }
    size_t roadCount() const { return roadCountValue; }

    vector<string> getLines() const {
        set<string> uniqueLines;
        for (const auto& entry : adjacency) {
            for (const Road& road : entry.second) uniqueLines.insert(road.lineName);
        }
        return vector<string>(uniqueLines.begin(), uniqueLines.end());
    }

    RouteResult findRoute(int startId, int endId, OptimizationMode mode,
                          const Bus& bus, float rushMultiplier) const {
        RouteResult result;

        if (!hasStop(startId) || !hasStop(endId)) {
            result.message = "Stop ID not found in network.";
            return result;
        }
        if (startId == endId) {
            result.found = true;
            result.stopIds.push_back(startId);
            result.message = "Origin and destination are the same stop.";
            return result;
        }

        struct PathBest {
            float primary = 0.0f;
            float secondary = 0.0f;
            int prevNode = -1;
            string prevLine;
            float distance = 0.0f;
            float time = 0.0f;
            int transfers = 0;
            Road via;
            bool hasPrev = false;
        };

        struct PQItem {
            float primary;
            float secondary;
            int node;
            string line;
            float distance;
            float time;
            int transfers;
        };

        struct PQCompare {
            bool operator()(const PQItem& a, const PQItem& b) const {
                if (a.primary != b.primary) return a.primary > b.primary;
                if (a.secondary != b.secondary) return a.secondary > b.secondary;
                return a.node > b.node;
            }
        };

        const float EPS = 1e-4f;
        map<pair<int, string>, PathBest> best;
        priority_queue<PQItem, vector<PQItem>, PQCompare> queue;

        PathBest startBest;
        best[{startId, ""}] = startBest;
        queue.push(PQItem{0.0f, 0.0f, startId, "", 0.0f, 0.0f, 0});

        while (!queue.empty()) {
            PQItem current = queue.top();
            queue.pop();

            auto bestIt = best.find({current.node, current.line});
            if (bestIt == best.end()) continue;
            if (current.primary > bestIt->second.primary + EPS) continue;
            if (std::fabs(current.primary - bestIt->second.primary) <= EPS &&
                current.secondary > bestIt->second.secondary + EPS) {
                continue;
            }

            for (const Road& road : adjacency.at(current.node)) {
                int extraTransfer =
                    (!current.line.empty() && current.line != road.lineName) ? 1 : 0;
                int newTransfers = current.transfers + extraTransfer;
                float legMinutes = bus.calculateTripTime(
                    road.distanceKm, road.speedLimitKmh, rushMultiplier);
                // [NEW] For fastest routing, each edge also costs one dwell;
                // the final stop's dwell is removed after reconstruction.
                if (mode == OptimizationMode::FASTEST) {
                    legMinutes += bus.getDwellMinutes();
                }
                float newDistance = current.distance + road.distanceKm;
                float newTime = current.time + legMinutes;

                float primary = 0.0f;
                float secondary = 0.0f;
                if (mode == OptimizationMode::FASTEST) {
                    primary = newTime;
                    secondary = newDistance;
                } else if (mode == OptimizationMode::SHORTEST) {
                    primary = newDistance;
                    secondary = newTime;
                } else {
                    primary = static_cast<float>(newTransfers);
                    secondary = newDistance;
                }

                pair<int, string> key{road.toId, road.lineName};
                auto existing = best.find(key);
                bool better = (existing == best.end()) ||
                    (primary < existing->second.primary - EPS) ||
                    (std::fabs(primary - existing->second.primary) <= EPS &&
                     secondary < existing->second.secondary - EPS);

                if (better) {
                    PathBest next;
                    next.primary = primary;
                    next.secondary = secondary;
                    next.prevNode = current.node;
                    next.prevLine = current.line;
                    next.distance = newDistance;
                    next.time = newTime;
                    next.transfers = newTransfers;
                    next.via = road;
                    next.hasPrev = true;
                    best[key] = next;

                    queue.push(PQItem{primary, secondary, road.toId, road.lineName,
                                      newDistance, newTime, newTransfers});
                }
            }
        }

        const PathBest* endBest = nullptr;
        pair<int, string> endKey;
        for (const auto& entry : best) {
            if (entry.first.first != endId) continue;
            if (endBest == nullptr ||
                entry.second.primary < endBest->primary - EPS ||
                (std::fabs(entry.second.primary - endBest->primary) <= EPS &&
                 entry.second.secondary < endBest->secondary - EPS)) {
                endBest = &entry.second;
                endKey = entry.first;
            }
        }

        if (endBest == nullptr) {
            result.message = "No route exists between stop " + to_string(startId) +
                             " and stop " + to_string(endId) + ".";
            return result;
        }

        vector<Road> reversedLegs;
        pair<int, string> cursor = endKey;
        while (true) {
            auto it = best.find(cursor);
            if (it == best.end() || !it->second.hasPrev) break;
            reversedLegs.push_back(it->second.via);
            cursor = pair<int, string>{it->second.prevNode, it->second.prevLine};
        }
        reverse(reversedLegs.begin(), reversedLegs.end());

        result.found = true;
        result.stopIds.push_back(startId);
        for (const Road& leg : reversedLegs) result.stopIds.push_back(leg.toId);
        result.legs = reversedLegs;
        result.transfers = endBest->transfers;
        result.totalDistanceKm = endBest->distance;

        int intermediateStops = static_cast<int>(result.stopIds.size()) - 2;
        float dwell = intermediateStops > 0
                          ? intermediateStops * bus.getDwellMinutes()
                          : 0.0f;
        if (mode == OptimizationMode::FASTEST) {
            result.totalMinutes = endBest->time - bus.getDwellMinutes();
        } else {
            result.totalMinutes = endBest->time + dwell;
        }
        result.message = "Route found.";
        return result;
    }
};

// ==========================================
// UNIT 4: TEMPLATES (Generic Programming)
// ==========================================
template <typename T>
class RouteOptimizer {
public:
    static T computeCumulativeDistance(const vector<T>& distances,
                                       int startIdx, int endIdx) {
        T total = 0;
        for (int i = startIdx + 1; i <= endIdx; ++i) {
            total += distances[i];
        }
        return total;
    }
};

// ==========================================
// UNIT 5: FILE HANDLING + MANAGER CLASS
// ==========================================
struct JourneyPlan {
    bool valid = false;
    RouteResult route;
    int busIndex = -1;
    float rushMultiplier = 1.0f;
    float fare = 0.0f;
    string fareStrategy;
    PassengerType passengerType = PassengerType::REGULAR;
    string originName;
    string destinationName;
};

class BusNetworkManager {
private:
    BusGraph graph;
    vector<unique_ptr<Bus>> fleet;
    map<string, Ticket> tickets;      // [NEW] active tickets keyed by ticket ID
    static int totalQueriesProcessed;
    static int nextPassengerId;

    static int parseStrictInt(const string& token, int lineNo, const string& field) {
        try {
            size_t pos = 0;
            int value = stoi(token, &pos);
            if (pos != token.size()) throw invalid_argument("trailing characters");
            return value;
        } catch (...) {
            throw InvalidInputException("Malformed " + field + " on line " +
                                        to_string(lineNo) + ": '" + token + "'");
        }
    }

    static float parseStrictFloat(const string& token, int lineNo,
                                  const string& field) {
        try {
            size_t pos = 0;
            float value = stof(token, &pos);
            if (pos != token.size() || !std::isfinite(value)) {
                throw invalid_argument("trailing characters");
            }
            return value;
        } catch (...) {
            throw InvalidInputException("Malformed " + field + " on line " +
                                        to_string(lineNo) + ": '" + token + "'");
        }
    }

    void seedRouteFileIfMissing(const string& filename) {
        ifstream check(filename, ios::binary | ios::ate);
        if (check.good() && check.tellg() > 0) return;

        ofstream outfile(filename);
        if (!outfile.is_open()) {
            throw RouteNotFoundException("Failed to create " + filename);
        }
        outfile << "# Pune Smart Bus Route Planner - network definition\n";
        outfile << "# STOP <id> <name>\n";
        outfile << "# EDGE <fromId> <toId> <distanceKm> <speedLimitKmh> <lineName>\n";
        outfile << "STOP 1 Pimpri\n";
        outfile << "STOP 2 Chinchwad\n";
        outfile << "STOP 3 Khadki\n";
        outfile << "STOP 4 Shivajinagar\n";
        outfile << "STOP 5 Civil_Court\n";
        outfile << "STOP 6 Swargate\n";
        outfile << "STOP 7 Vanaz\n";
        outfile << "STOP 8 Kothrud\n";
        outfile << "STOP 9 Deccan_Gymkhana\n";
        outfile << "STOP 10 PMC\n";
        outfile << "STOP 11 Pune_Station\n";
        outfile << "STOP 12 Kalyani_Nagar\n";
        outfile << "STOP 13 Ramwadi\n";
        outfile << "STOP 14 Katraj\n";
        outfile << "STOP 15 Camp\n";
        outfile << "STOP 16 Bund_Garden\n";
        outfile << "STOP 17 Viman_Nagar\n";
        outfile << "STOP 18 Kharadi\n";
        outfile << "STOP 19 Pune_Airport\n";
        outfile << "STOP 20 Hinjawadi\n";
        outfile << "STOP 21 Wakad\n";
        outfile << "STOP 22 Baner\n";
        outfile << "STOP 23 Aundh\n";
        outfile << "STOP 24 Hadapsar\n";
        outfile << "STOP 25 Magarpatta\n";
        outfile << "EDGE 1 2 3.0 40 PurpleLine\n";
        outfile << "EDGE 2 3 4.5 35 PurpleLine\n";
        outfile << "EDGE 3 4 5.0 35 PurpleLine\n";
        outfile << "EDGE 4 5 2.5 30 PurpleLine\n";
        outfile << "EDGE 5 6 2.0 30 PurpleLine\n";
        outfile << "EDGE 7 8 4.0 35 AquaLine\n";
        outfile << "EDGE 8 9 3.5 35 AquaLine\n";
        outfile << "EDGE 9 10 2.2 30 AquaLine\n";
        outfile << "EDGE 10 5 1.8 30 AquaLine\n";
        outfile << "EDGE 5 11 2.5 30 AquaLine\n";
        outfile << "EDGE 11 12 4.0 40 AquaLine\n";
        outfile << "EDGE 12 13 5.0 45 AquaLine\n";
        outfile << "EDGE 14 6 6.5 35 RedLine\n";
        outfile << "EDGE 6 15 2.2 30 RedLine\n";
        outfile << "EDGE 15 16 2.0 30 RedLine\n";
        outfile << "EDGE 16 17 3.2 35 RedLine\n";
        outfile << "EDGE 17 18 4.5 45 RedLine\n";
        outfile << "EDGE 18 19 4.0 50 RedLine\n";
        outfile << "EDGE 20 21 4.0 40 GreenLine\n";
        outfile << "EDGE 21 22 3.5 35 GreenLine\n";
        outfile << "EDGE 22 23 3.0 35 GreenLine\n";
        outfile << "EDGE 23 4 5.5 40 GreenLine\n";
        outfile << "EDGE 4 16 5.0 40 GreenLine\n";
        outfile << "EDGE 16 24 6.0 45 GreenLine\n";
        outfile << "EDGE 24 25 3.0 40 GreenLine\n";
        outfile.close();
    }

    Bus* findBusById(const string& id) {
        for (auto& bus : fleet) {
            if (bus->getId() == id) return bus.get();
        }
        return nullptr;
    }

    vector<string> splitByTab(const string& line) const {
        vector<string> fields;
        string field;
        istringstream stream(line);
        while (getline(stream, field, '\t')) fields.push_back(field);
        return fields;
    }

public:
    BusNetworkManager() {
        loadNetworkFromFile("routes.txt");
        Ticket::syncSerialFromLog("trip_log.txt");
        fleet.push_back(BusFactory::create(BusKind::REGULAR));
        fleet.push_back(BusFactory::create(BusKind::EXPRESS));
        fleet.push_back(BusFactory::create(BusKind::AC));
        fleet.push_back(BusFactory::create(BusKind::ELECTRIC));
    }

    const BusGraph& getGraph() const { return graph; }
    const vector<unique_ptr<Bus>>& getFleet() const { return fleet; }

    // [FIXED] fully validated file loader
    void loadNetworkFromFile(const string& filename) {
        seedRouteFileIfMissing(filename);

        ifstream infile(filename);
        if (!infile.is_open()) {
            throw RouteNotFoundException("Failed to open " + filename);
        }

        BusGraph newGraph;
        string rawLine;
        int lineNo = 0;

        while (getline(infile, rawLine)) {
            ++lineNo;
            string line = trimCopy(rawLine);
            if (line.empty() || line[0] == '#') continue;

            istringstream iss(line);
            string tag;
            iss >> tag;

            if (tag == "STOP") {
                string idToken, name;
                if (!(iss >> idToken >> name)) {
                    throw InvalidInputException("Malformed STOP on line " +
                                                to_string(lineNo));
                }
                string extra;
                if (iss >> extra) {
                    throw InvalidInputException("Unexpected token on STOP line " +
                                                to_string(lineNo) +
                                                " (use underscores for spaces).");
                }
                int id = parseStrictInt(idToken, lineNo, "stop ID");
                if (id <= 0) {
                    throw InvalidInputException("Negative or zero stop ID on line " +
                                                to_string(lineNo));
                }
                newGraph.addStop(id, name);
            } else if (tag == "EDGE") {
                string fromToken, toToken, distanceToken, speedToken, lineName;
                if (!(iss >> fromToken >> toToken >> distanceToken >>
                      speedToken >> lineName)) {
                    throw InvalidInputException("Malformed EDGE on line " +
                                                to_string(lineNo));
                }
                string extra;
                if (iss >> extra) {
                    throw InvalidInputException("Unexpected token on EDGE line " +
                                                to_string(lineNo));
                }
                int fromId = parseStrictInt(fromToken, lineNo, "from stop ID");
                int toId = parseStrictInt(toToken, lineNo, "to stop ID");
                float distance = parseStrictFloat(distanceToken, lineNo, "distance");
                float speed = parseStrictFloat(speedToken, lineNo, "speed limit");

                if (distance <= 0.0f) {
                    throw InvalidInputException("Negative or zero distance on line " +
                                                to_string(lineNo));
                }
                if (speed <= 0.0f || speed > 200.0f) {
                    throw InvalidInputException("Invalid speed limit on line " +
                                                to_string(lineNo));
                }
                if (!newGraph.hasStop(fromId) || !newGraph.hasStop(toId)) {
                    throw InvalidInputException("EDGE references unknown stop on line " +
                                                to_string(lineNo));
                }
                newGraph.addRoad(fromId, toId, distance, speed, lineName);
            } else {
                throw InvalidInputException("Unknown directive '" + tag +
                                            "' on line " + to_string(lineNo));
            }
        }

        if (newGraph.stopCount() == 0) {
            throw InvalidInputException("No STOP entries found in " + filename);
        }
        if (newGraph.roadCount() == 0) {
            throw InvalidInputException("No EDGE entries found in " + filename);
        }

        graph = std::move(newGraph);
    }

    // [NEW] Time-of-day rush-hour model (replaces manual congestion input)
    static float rushMultiplierForHour(int hour) {
        if (hour < 0 || hour > 23) {
            throw InvalidInputException("Hour must be between 0 and 23.");
        }
        if ((hour >= 7 && hour <= 10) || (hour >= 17 && hour <= 20)) return 1.80f;
        if (hour >= 11 && hour <= 16) return 1.20f;
        return 0.85f;
    }

    void displayNetwork() const {
        printTitle("PUNE BUS NETWORK");

        cout << "\n  " << Style::paint("STOPS (" + to_string(graph.stopCount()) + ")",
                                       Style::BOLD + Style::YELLOW) << "\n";
        int index = 0;
        for (const auto& entry : graph.getStops()) {
            string label = "[" + fitLeft(to_string(entry.first), 2) + "] " +
                           fitRight(entry.second.getName(), 21);
            cout << "  " << Style::paint(label, index % 2 == 0 ? Style::WHITE
                                                             : Style::DIM);
            if (index % 2 == 1) cout << "\n";
            ++index;
        }
        if (index % 2 == 1) cout << "\n";

        cout << "\n  " << Style::paint("LINES & ROADS",
                                       Style::BOLD + Style::YELLOW) << "\n";
        for (const string& line : graph.getLines()) {
            cout << "\n  " << Style::paint("* " + line,
                                           Style::BOLD + lineColor(line)) << "\n";

            vector<Road> roads;
            for (const auto& entry : graph.getAdjacency()) {
                for (const Road& road : entry.second) {
                    if (road.lineName == line && road.fromId < road.toId) {
                        roads.push_back(road);
                    }
                }
            }

            map<int, vector<Road>> byStop;
            for (const Road& road : roads) {
                byStop[road.fromId].push_back(road);
                byStop[road.toId].push_back(road);
            }

            int current = -1;
            for (const auto& entry : byStop) {
                if (entry.second.size() == 1) {
                    current = entry.first;
                    break;
                }
            }
            if (current == -1 && !byStop.empty()) current = byStop.begin()->first;

            set<pair<int, int>> visited;
            while (current != -1) {
                const Road* nextRoad = nullptr;
                int nextStop = -1;
                for (const Road& road : byStop[current]) {
                    int other = (road.fromId == current) ? road.toId : road.fromId;
                    pair<int, int> key(min(current, other), max(current, other));
                    if (visited.count(key) == 0) {
                        nextRoad = &road;
                        nextStop = other;
                        break;
                    }
                }
                if (nextRoad == nullptr) break;

                visited.insert(pair<int, int>(min(current, nextStop),
                                              max(current, nextStop)));

                const string& fromName = graph.getStop(current).getName();
                const string& toName = graph.getStop(nextStop).getName();
                cout << "      " << Style::paint(fitRight(fromName, 18), Style::WHITE)
                     << Style::paint(" <-> ", Style::DIM)
                     << Style::paint(fitRight(toName, 18), Style::WHITE)
                     << " " << Style::paint(fitLeft(numberString(nextRoad->distanceKm, 1) + " km", 7),
                                            Style::GREEN)
                     << " " << Style::paint("(" + numberString(nextRoad->speedLimitKmh, 0) + " km/h)",
                                            Style::DIM)
                     << "\n";
                current = nextStop;
            }
        }
        printRule();
    }

    void displayFleet() const {
        printSubtitle("AVAILABLE FLEET");

        vector<int> widths{3, 22, 9, 9, 9};
        printTableSeparator(widths);
        printTableRow({fitLeft("#", 3), "Bus Type", "Bus ID",
                       fitLeft("Seats", 9), fitLeft("Rs/km", 9)}, widths);
        printTableSeparator(widths);

        for (size_t i = 0; i < fleet.size(); ++i) {
            const Bus& bus = *fleet[i];
            string seats = to_string(bus.getSeatsBooked()) + "/" +
                           to_string(bus.getCapacity());
            printTableRow({fitLeft(to_string(i + 1), 3), bus.getType(), bus.getId(),
                           fitLeft(seats, 9), fitLeft(numberString(bus.getFareRate(), 2), 9)},
                          widths, {"", Style::CYAN, Style::DIM, "", Style::GREEN});
        }
        printTableSeparator(widths);
    }

    JourneyPlan planJourney(int startId, int destId, int modeChoice, int hour,
                            int busChoice, PassengerType passengerType) {
        if (!graph.hasStop(startId)) {
            throw InvalidStopException("Origin stop ID " + to_string(startId) +
                                       " does not exist.");
        }
        if (!graph.hasStop(destId)) {
            throw InvalidStopException("Destination stop ID " + to_string(destId) +
                                       " does not exist.");
        }
        if (startId == destId) {
            throw InvalidInputException("Origin and destination must be different.");
        }

        OptimizationMode mode;
        try {
            mode = parseOptimizationMode(modeChoice);
        } catch (...) {
            throw InvalidInputException("Optimization mode must be 1, 2, or 3.");
        }

        float rush = rushMultiplierForHour(hour);

        if (busChoice < 1 || busChoice > static_cast<int>(fleet.size())) {
            throw InvalidInputException("Bus choice must be between 1 and " +
                                        to_string(fleet.size()) + ".");
        }
        Bus* bus = fleet.at(static_cast<size_t>(busChoice - 1)).get();

        RouteResult route = graph.findRoute(startId, destId, mode, *bus, rush);
        if (!route.found) {
            throw RouteNotFoundException(route.message);
        }

        // [NEW] Template usage for cumulative distance
        vector<float> segmentDistances;
        segmentDistances.push_back(0.0f);
        for (const Road& leg : route.legs) segmentDistances.push_back(leg.distanceKm);
        float templateDistance = RouteOptimizer<float>::computeCumulativeDistance(
            segmentDistances, 0, static_cast<int>(segmentDistances.size()) - 1);

        unique_ptr<FareStrategy> fareStrategy =
            FareStrategyFactory::create(passengerType, bus->isExpressBus());
        FareContext fareContext{templateDistance, bus->getFareRate(),
                                bus->getBoardingFee()};
        float fare = fareStrategy->calculate(fareContext);

        JourneyPlan plan;
        plan.valid = true;
        plan.route = route;
        plan.busIndex = busChoice - 1;
        plan.rushMultiplier = rush;
        plan.fare = fare;
        plan.fareStrategy = fareStrategy->name();
        plan.passengerType = passengerType;
        plan.originName = graph.getStop(startId).getName();
        plan.destinationName = graph.getStop(destId).getName();

        printTitle("JOURNEY SUMMARY");
        printKV("Optimization Mode", optimizationModeToString(mode));
        printKV("Origin", plan.originName);
        printKV("Destination", plan.destinationName);
        printKV("Bus Allocated", bus->getType() + " [" + bus->getId() + "]");
        printKV("Intermediate Stops",
                to_string(static_cast<int>(route.stopIds.size()) - 2));
        printKV("Transfers", to_string(route.transfers));
        printKV("Corridor Distance", numberString(templateDistance, 2) + " km");

        string hourText = (hour < 10 ? "0" : "") + to_string(hour) + ":00";
        string rushLabel = rush >= 1.5f ? "RUSH"
                          : (rush >= 1.1f ? "BUSY" : "OFF-PEAK");
        string rushColor = rush >= 1.5f ? Style::RED
                          : (rush >= 1.1f ? Style::YELLOW : Style::GREEN);
        printKV("Time of Day", hourText + "  " +
                Style::paint("[" + rushLabel + " x" + numberString(rush, 2) + "]",
                             rushColor));
        printKV("Estimated Time", numberString(route.totalMinutes, 1) +
                " minutes (includes dwell)");
        printKV("Fare Strategy", fareStrategy->name());
        printKV("Estimated Fare", Style::paint("Rs. " + numberString(fare, 2),
                                               Style::GREEN));
        printKV("Seat Availability", to_string(bus->getAvailableSeats()) + "/" +
                to_string(bus->getCapacity()));

        printSubtitle("ROUTE ITINERARY");
        for (size_t i = 0; i < route.stopIds.size(); ++i) {
            const RouteStop& stop = graph.getStop(route.stopIds[i]);
            bool isStart = (i == 0);
            bool isEnd = (i + 1 == route.stopIds.size());
            string tag = isStart ? "START" : (isEnd ? "END" : "STOP");
            string tagColor = (isStart || isEnd) ? Style::GREEN : Style::DIM;

            cout << "  " << Style::paint(fitRight(tag, 5), tagColor)
                 << " " << Style::paint(to_string(i + 1) + ". " + stop.getName(),
                                        Style::BOLD) << "\n";

            if (i < route.legs.size()) {
                const Road& leg = route.legs[i];
                cout << "        " << Style::paint("|", Style::DIM) << " "
                     << Style::paint(leg.lineName, lineColor(leg.lineName))
                     << Style::paint("  " + numberString(leg.distanceKm, 1) + " km  " +
                                         numberString(leg.speedLimitKmh, 0) + " km/h",
                                     Style::DIM)
                     << "\n";

                if (i + 1 < route.legs.size() &&
                    route.legs[i + 1].lineName != leg.lineName) {
                    cout << "        " << Style::paint("|", Style::DIM) << " "
                         << Style::paint("TRANSFER at " +
                                             graph.getStop(route.stopIds[i + 1]).getName(),
                                         Style::YELLOW)
                         << "\n";
                }
            }
        }
        printRule();

        totalQueriesProcessed++;
        return plan;
    }

    Ticket bookTicket(const JourneyPlan& plan, const string& passengerName) {
        if (!plan.valid) {
            throw InvalidInputException("Cannot book an invalid journey plan.");
        }
        string cleanName = trimCopy(passengerName);
        if (cleanName.empty()) {
            throw InvalidInputException("Passenger name cannot be empty.");
        }

        Bus* bus = fleet.at(static_cast<size_t>(plan.busIndex)).get();
        bus->reserveSeat(); // throws BusFullException when the bus is full
        try {
            Passenger passenger(++nextPassengerId, cleanName, plan.passengerType);
            Ticket ticket(passenger, bus->getId(), bus->getType(),
                          plan.originName, plan.destinationName,
                          plan.route.totalDistanceKm, plan.route.totalMinutes,
                          plan.fare, plan.fareStrategy);

            ofstream logfile("trip_log.txt", ios::app);
            if (!logfile.is_open()) {
                throw RouteNotFoundException("Unable to open trip_log.txt for writing.");
            }
            logfile << ticket.toLogLine() << "\n";
            if (!logfile) {
                throw RouteNotFoundException("Failed to write to trip_log.txt.");
            }
            logfile.close();

            tickets.emplace(ticket.getId(), ticket);
            cout << "  " << Style::paint("[Log saved to trip_log.txt]",
                                         Style::DIM) << "\n";
            return ticket;
        } catch (...) {
            bus->releaseSeat(); // rollback if logging or storage fails
            throw;
        }
    }

    bool cancelTicket(const string& ticketId) {
        auto it = tickets.find(ticketId);
        if (it == tickets.end()) {
            throw InvalidInputException("Ticket ID '" + ticketId + "' was not found.");
        }
        Bus* bus = findBusById(it->second.getBusId());
        if (bus == nullptr) {
            throw InvalidInputException("Ticket references an unknown bus.");
        }
        bus->releaseSeat();

        // [NEW] Append a cancellation marker so statistics can exclude refunded trips
        ofstream logfile("trip_log.txt", ios::app);
        if (logfile.is_open()) {
            logfile << "CANCEL\t" << ticketId << "\n";
        }

        cout << "\n  " << Style::paint("Ticket " + ticketId + " cancelled.", Style::GREEN)
             << " Seat released on " << bus->getType()
             << " [" << bus->getId() << "].\n";
        tickets.erase(it);
        return true;
    }

    // [NEW] Compare mode: one table for every bus type
    void compareFleet(int startId, int destId, int modeChoice, int hour,
                      PassengerType passengerType) {
        if (!graph.hasStop(startId)) {
            throw InvalidStopException("Origin stop ID " + to_string(startId) +
                                       " does not exist.");
        }
        if (!graph.hasStop(destId)) {
            throw InvalidStopException("Destination stop ID " + to_string(destId) +
                                       " does not exist.");
        }
        if (startId == destId) {
            throw InvalidInputException("Origin and destination must be different.");
        }

        OptimizationMode mode;
        try {
            mode = parseOptimizationMode(modeChoice);
        } catch (...) {
            throw InvalidInputException("Optimization mode must be 1, 2, or 3.");
        }
        float rush = rushMultiplierForHour(hour);

        printTitle("FLEET COMPARISON");
        printKV("Optimization Mode", optimizationModeToString(mode));
        printKV("Time of Day", (hour < 10 ? "0" : "") + to_string(hour) + ":00" +
                "  " + Style::paint("[rush x" + numberString(rush, 2) + "]",
                                    rush >= 1.5f ? Style::RED
                                    : (rush >= 1.1f ? Style::YELLOW : Style::GREEN)));
        printKV("Passenger Type", passengerTypeToString(passengerType));

        vector<int> widths{22, 10, 10, 9, 9, 18};
        printSubtitle("OPTIONS");
        printTableSeparator(widths);
        printTableRow({"Bus Type", fitLeft("Distance", 10), fitLeft("Time", 10),
                       fitLeft("Transfers", 9), fitLeft("Fare", 9), "Strategy"},
                      widths);
        printTableSeparator(widths);

        for (const auto& busPtr : fleet) {
            const Bus& bus = *busPtr;
            RouteResult route = graph.findRoute(startId, destId, mode, bus, rush);
            if (!route.found) {
                printTableRow({bus.getType(), fitLeft("-", 10), fitLeft("-", 10),
                               fitLeft("-", 9), fitLeft("-", 9), "No route"},
                              widths, {Style::CYAN, "", "", "", "", Style::RED});
                continue;
            }

            unique_ptr<FareStrategy> strategy =
                FareStrategyFactory::create(passengerType, bus.isExpressBus());
            float fare = strategy->calculate(
                FareContext{route.totalDistanceKm, bus.getFareRate(),
                            bus.getBoardingFee()});

            printTableRow({bus.getType(),
                           fitLeft(numberString(route.totalDistanceKm, 1) + " km", 10),
                           fitLeft(numberString(route.totalMinutes, 1) + " min", 10),
                           fitLeft(to_string(route.transfers), 9),
                           fitLeft(numberString(fare, 2), 9),
                           strategy->name()},
                          widths,
                          {Style::CYAN, "", "", "", Style::GREEN, ""});
        }
        printTableSeparator(widths);

        totalQueriesProcessed++;
    }

    // [NEW] Trip history + simple analytics read from trip_log.txt
    void showTripHistory() const {
        ifstream infile("trip_log.txt");
        if (!infile.is_open()) {
            cout << "\nNo trip history found (trip_log.txt does not exist yet).\n";
            return;
        }

        vector<string> lines;
        string line;
        while (getline(infile, line)) {
            if (!trimCopy(line).empty()) lines.push_back(line);
        }

        set<string> cancelledIds;
        for (const string& entry : lines) {
            if (entry.compare(0, 7, "CANCEL\t") == 0) {
                vector<string> cancelFields = splitByTab(entry);
                if (cancelFields.size() >= 2) cancelledIds.insert(cancelFields[1]);
            }
        }

        map<string, int> routeCounts;
        double revenue = 0.0;
        double totalDistance = 0.0;
        int parsedTrips = 0;

        for (const string& entry : lines) {
            vector<string> fields = splitByTab(entry);
            if (fields.size() != 10) continue;
            if (cancelledIds.count(fields[0]) != 0) continue;
            try {
                double distance = stod(fields[6]);
                double minutes = stod(fields[7]);
                double fare = stod(fields[8]);
                if (distance < 0.0 || minutes < 0.0 || fare < 0.0) continue;
                ++parsedTrips;
                totalDistance += distance;
                revenue += fare;
                routeCounts[fields[3] + " -> " + fields[4]]++;
            } catch (...) {
                continue; // skip malformed historical lines
            }
        }

        string busiestRoute = "N/A";
        int busiestCount = 0;
        for (const auto& entry : routeCounts) {
            if (entry.second > busiestCount) {
                busiestRoute = entry.first;
                busiestCount = entry.second;
            }
        }

        printTitle("TRIP HISTORY & STATS");
        printKV("Total Trips", to_string(parsedTrips));
        printKV("Total Revenue", Style::paint("Rs. " +
                numberString(static_cast<float>(revenue), 2), Style::GREEN));
        printKV("Busiest Route", busiestRoute +
                (busiestCount > 0
                     ? " (" + to_string(busiestCount) +
                           (busiestCount == 1 ? " trip)" : " trips)")
                     : ""));
        printKV("Total Distance", numberString(static_cast<float>(totalDistance), 1) + " km");
        printKV("Cancelled Tickets", to_string(cancelledIds.size()));

        printSubtitle("RECENT TRIPS");
        vector<int> widths{11, 38, 22, 10, 10};
        int shownTrips = 0;
        for (size_t i = lines.size(); i > 0 && shownTrips < 5; --i) {
            vector<string> fields = splitByTab(lines[i - 1]);
            if (fields.size() != 10) continue;
            if (cancelledIds.count(fields[0]) != 0) continue;

            if (shownTrips == 0) {
                printTableSeparator(widths);
                printTableRow({"Ticket", "Route", "Bus",
                               fitLeft("ETA", 10), fitLeft("Fare", 10)}, widths);
                printTableSeparator(widths);
            }
            printTableRow({fields[0], fields[3] + " -> " + fields[4], fields[5],
                           fitLeft(fields[7] + " min", 10),
                           fitLeft("Rs. " + fields[8], 10)},
                          widths, {Style::DIM, "", Style::CYAN, "", Style::GREEN});
            ++shownTrips;
        }
        if (shownTrips == 0) {
            cout << "  " << Style::paint("No completed trips recorded yet.", Style::DIM) << "\n";
        } else {
            printTableSeparator(widths);
        }
        printRule();
    }

    void showSystemStats() const {
        printTitle("SYSTEM ANALYTICS");
        printKV("Planning Queries Served", to_string(totalQueriesProcessed));
        printKV("Active Fleets in Memory", to_string(Bus::getActiveBusCount()));
        printKV("Active Tickets", to_string(tickets.size()));
        printKV("Network Size", to_string(graph.stopCount()) + " stops, " +
                to_string(graph.roadCount()) + " roads, " +
                to_string(graph.getLines().size()) + " lines");
        printRule();
    }
};

int BusNetworkManager::totalQueriesProcessed = 0;
int BusNetworkManager::nextPassengerId = 0;

// ==========================================
// [NEW] INPUT HELPERS (validated cin reads)
// ==========================================
template <typename T>
static bool readNumber(T& out) {
    if (cin >> out) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return true;
    }
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return false;
}

template <typename T>
static bool askNumber(const string& prompt, T& out) {
    cout << "\n  " << Style::paint(prompt, Style::BOLD + Style::CYAN);
    return readNumber(out);
}

static string readLineValue(const string& prompt) {
    cout << "  " << Style::paint(prompt, Style::BOLD + Style::CYAN);
    string value;
    if (!getline(cin, value)) {
        throw InvalidInputException("Input stream closed unexpectedly.");
    }
    return trimCopy(value);
}

// ==========================================
// [NEW] UNIT 4/7: ASSERT-BASED SELF TESTS
// ==========================================
static void runSelfTests() {
    // ---- ETA tests ----
    RegularBus regular("T-REG");
    ExpressBus express("T-EXP");
    ACBus ac("T-AC");
    ElectricBus electric("T-ELE");

    float regularTime = regular.calculateTripTime(10.0f, 60.0f, 1.0f);
    float expressTime = express.calculateTripTime(10.0f, 60.0f, 1.0f);
    assert(regularTime > 0.0f);
    assert(expressTime > 0.0f);
    assert(expressTime < regularTime);
    assert(regular.calculateTripTime(10.0f, 60.0f, 2.0f) > regularTime);
    assert(ac.calculateTripTime(10.0f, 60.0f, 1.0f) > 0.0f);
    assert(electric.calculateTripTime(10.0f, 60.0f, 1.0f) > 0.0f);

    // ---- Fare tests ----
    FareContext context{10.0f, 2.0f, 10.0f};
    RegularFare regularFare;
    ExpressFare expressFare;
    StudentDiscount student;
    SeniorDiscount senior;

    float regularAmount = regularFare.calculate(context);
    float expressAmount = expressFare.calculate(context);
    float studentAmount = student.calculate(context);
    float seniorAmount = senior.calculate(context);

    assert(regularAmount > 0.0f);
    assert(expressAmount > regularAmount);
    assert(studentAmount < regularAmount);
    assert(seniorAmount < regularAmount);
    assert(studentAmount < seniorAmount);
    assert(seniorAmount < expressAmount);

    // ---- Dijkstra: same start and end ----
    BusGraph graph;
    graph.addStop(1, "A");
    graph.addStop(2, "B");
    graph.addStop(3, "C");
    graph.addStop(4, "Island");
    graph.addStop(6, "X");
    graph.addStop(7, "Y");

    graph.addRoad(1, 2, 1.0f, 20.0f, "SlowLine");
    graph.addRoad(2, 3, 1.0f, 20.0f, "SlowLine");
    graph.addRoad(1, 6, 2.0f, 100.0f, "FastLine");
    graph.addRoad(6, 3, 2.0f, 100.0f, "FastLine");

    RouteResult same = graph.findRoute(1, 1, OptimizationMode::FASTEST, regular, 1.0f);
    assert(same.found);
    assert(same.stopIds.size() == 1);
    assert(same.totalDistanceKm == 0.0f);
    assert(same.transfers == 0);

    // ---- Dijkstra: unreachable stop ----
    RouteResult unreachable =
        graph.findRoute(1, 4, OptimizationMode::FASTEST, regular, 1.0f);
    assert(!unreachable.found);

    // ---- Dijkstra: invalid stop ID ----
    RouteResult invalid =
        graph.findRoute(99, 1, OptimizationMode::FASTEST, regular, 1.0f);
    assert(!invalid.found);

    // ---- Dijkstra: shortest vs fastest route choice ----
    RouteResult shortest =
        graph.findRoute(1, 3, OptimizationMode::SHORTEST, express, 1.0f);
    assert(shortest.found);
    assert(shortest.stopIds.size() == 3);
    assert(shortest.stopIds[1] == 2);
    assert(shortest.totalDistanceKm == 2.0f);

    RouteResult fastest =
        graph.findRoute(1, 3, OptimizationMode::FASTEST, express, 1.0f);
    assert(fastest.found);
    assert(fastest.stopIds.size() == 3);
    assert(fastest.stopIds[1] == 6);

    // ---- Dijkstra: transfer counting ----
    BusGraph transferGraph;
    transferGraph.addStop(1, "A");
    transferGraph.addStop(2, "B");
    transferGraph.addStop(3, "C");
    transferGraph.addRoad(1, 2, 1.0f, 30.0f, "LineOne");
    transferGraph.addRoad(2, 3, 1.0f, 30.0f, "LineTwo");

    RouteResult transferRoute = transferGraph.findRoute(
        1, 3, OptimizationMode::FEWEST_TRANSFERS, regular, 1.0f);
    assert(transferRoute.found);
    assert(transferRoute.transfers == 1);

    // ---- Graph validation: duplicate ID, negative distance, self-loop ----
    bool duplicateThrown = false;
    try {
        graph.addStop(1, "Duplicate");
    } catch (const InvalidInputException&) {
        duplicateThrown = true;
    }
    assert(duplicateThrown);

    bool negativeDistanceThrown = false;
    try {
        graph.addRoad(1, 2, -3.0f, 30.0f, "BadLine");
    } catch (const InvalidInputException&) {
        negativeDistanceThrown = true;
    }
    assert(negativeDistanceThrown);

    bool selfLoopThrown = false;
    try {
        graph.addRoad(1, 1, 1.0f, 30.0f, "BadLine");
    } catch (const InvalidInputException&) {
        selfLoopThrown = true;
    }
    assert(selfLoopThrown);

    // ---- Booking: seat capacity, BusFullException, cancellation rollback ----
    RegularBus fullBus("T-FULL");
    for (int i = 0; i < fullBus.getCapacity(); ++i) fullBus.reserveSeat();
    assert(fullBus.getAvailableSeats() == 0);
    bool fullThrown = false;
    try {
        fullBus.reserveSeat();
    } catch (const BusFullException&) {
        fullThrown = true;
    }
    assert(fullThrown);
    fullBus.releaseSeat();
    assert(fullBus.getAvailableSeats() == 1);

    cout << "All assert-based self-tests passed.\n";
}

// ==========================================
// [NEW] UNIT 8: JSON API MODE FOR THE WEB UI
// ==========================================
class ScopedCoutSilencer {
    ostringstream captured;
    streambuf* oldBuffer;

public:
    ScopedCoutSilencer() : oldBuffer(cout.rdbuf(captured.rdbuf())) {}
    ~ScopedCoutSilencer() { cout.rdbuf(oldBuffer); }
    ScopedCoutSilencer(const ScopedCoutSilencer&) = delete;
    ScopedCoutSilencer& operator=(const ScopedCoutSilencer&) = delete;
};

static string jsonEscape(const string& value) {
    ostringstream out;
    for (char ch : value) {
        switch (ch) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (static_cast<unsigned char>(ch) < 0x20) {
                    out << "\\u" << hex << setw(4) << setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(ch))
                        << dec << setfill(' ');
                } else {
                    out << ch;
                }
        }
    }
    return out.str();
}

static string jsonGetRaw(const string& json, const string& key) {
    string needle = "\"" + key + "\"";
    size_t pos = json.find(needle);
    if (pos == string::npos) return "";
    pos = json.find(':', pos + needle.size());
    if (pos == string::npos) return "";
    ++pos;
    while (pos < json.size() && isspace(static_cast<unsigned char>(json[pos]))) ++pos;
    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        ++pos;
        string out;
        bool escaping = false;
        for (; pos < json.size(); ++pos) {
            char ch = json[pos];
            if (escaping) {
                switch (ch) {
                    case 'n': out += '\n'; break;
                    case 'r': out += '\r'; break;
                    case 't': out += '\t'; break;
                    case '"': out += '"'; break;
                    case '\\': out += '\\'; break;
                    case '/': out += '/'; break;
                    default: out += ch; break;
                }
                escaping = false;
            } else if (ch == '\\') {
                escaping = true;
            } else if (ch == '"') {
                break;
            } else {
                out += ch;
            }
        }
        return out;
    }

    string out;
    while (pos < json.size() && json[pos] != ',' && json[pos] != '}' &&
           !isspace(static_cast<unsigned char>(json[pos]))) {
        out += json[pos++];
    }
    return out;
}

static int jsonGetInt(const string& json, const string& key, int fallback) {
    string raw = jsonGetRaw(json, key);
    if (raw.empty()) return fallback;
    try {
        size_t used = 0;
        int value = stoi(raw, &used);
        if (used != raw.size()) return fallback;
        return value;
    } catch (...) {
        return fallback;
    }
}

static string buildNetworkJson(const BusNetworkManager& manager) {
    const BusGraph& graph = manager.getGraph();
    const vector<unique_ptr<Bus>>& fleet = manager.getFleet();

    ostringstream out;
    out << "{\"ok\":true,\"network\":{";

    out << "\"stops\":[";
    bool first = true;
    for (const auto& entry : graph.getStops()) {
        if (!first) out << ',';
        first = false;
        out << "{\"id\":" << entry.first
            << ",\"name\":\"" << jsonEscape(entry.second.getName()) << "\"}";
    }
    out << "],\"lines\":[";
    first = true;
    for (const string& line : graph.getLines()) {
        if (!first) out << ',';
        first = false;
        out << "\"" << jsonEscape(line) << "\"";
    }
    out << "],\"roads\":[";
    first = true;
    for (const auto& entry : graph.getAdjacency()) {
        for (const Road& road : entry.second) {
            if (road.fromId >= road.toId) continue;
            if (!first) out << ',';
            first = false;
            out << "{\"from\":" << road.fromId
                << ",\"to\":" << road.toId
                << ",\"fromName\":\"" << jsonEscape(graph.getStop(road.fromId).getName()) << "\""
                << ",\"toName\":\"" << jsonEscape(graph.getStop(road.toId).getName()) << "\""
                << ",\"distance\":" << numberString(road.distanceKm, 1)
                << ",\"speed\":" << numberString(road.speedLimitKmh, 0)
                << ",\"line\":\"" << jsonEscape(road.lineName) << "\"}";
        }
    }
    out << "],\"fleet\":[";
    for (size_t i = 0; i < fleet.size(); ++i) {
        if (i > 0) out << ',';
        const Bus& bus = *fleet[i];
        out << "{\"index\":" << (i + 1)
            << ",\"type\":\"" << jsonEscape(bus.getType()) << "\""
            << ",\"id\":\"" << jsonEscape(bus.getId()) << "\""
            << ",\"seatsBooked\":" << bus.getSeatsBooked()
            << ",\"capacity\":" << bus.getCapacity()
            << ",\"available\":" << bus.getAvailableSeats()
            << ",\"fareRate\":" << numberString(bus.getFareRate(), 2)
            << ",\"boardingFee\":" << numberString(bus.getBoardingFee(), 2)
            << ",\"speed\":" << numberString(bus.getBaseSpeed(), 0)
            << ",\"express\":" << (bus.isExpressBus() ? "true" : "false")
            << "}";
    }
    out << "]}}";
    return out.str();
}

static string buildPlanJson(const JourneyPlan& plan,
                            const BusNetworkManager& manager,
                            int modeChoice, int hour) {
    const BusGraph& graph = manager.getGraph();
    const Bus& bus = *manager.getFleet().at(static_cast<size_t>(plan.busIndex));
    OptimizationMode mode = parseOptimizationMode(modeChoice);
    string rushLabel = plan.rushMultiplier >= 1.5f
                           ? "RUSH"
                           : (plan.rushMultiplier >= 1.1f ? "BUSY" : "OFF-PEAK");

    ostringstream out;
    out << "{\"ok\":true,\"plan\":{";
    out << "\"origin\":\"" << jsonEscape(plan.originName) << "\""
        << ",\"destination\":\"" << jsonEscape(plan.destinationName) << "\""
        << ",\"mode\":\"" << jsonEscape(optimizationModeToString(mode)) << "\""
        << ",\"hour\":" << hour
        << ",\"busIndex\":" << (plan.busIndex + 1)
        << ",\"bus\":\"" << jsonEscape(bus.getType() + " [" + bus.getId() + "]") << "\""
        << ",\"busType\":\"" << jsonEscape(bus.getType()) << "\""
        << ",\"busId\":\"" << jsonEscape(bus.getId()) << "\""
        << ",\"rush\":" << numberString(plan.rushMultiplier, 2)
        << ",\"rushLabel\":\"" << rushLabel << "\""
        << ",\"distance\":" << numberString(plan.route.totalDistanceKm, 2)
        << ",\"minutes\":" << numberString(plan.route.totalMinutes, 1)
        << ",\"transfers\":" << plan.route.transfers
        << ",\"intermediateStops\":" << (static_cast<int>(plan.route.stopIds.size()) - 2)
        << ",\"fare\":" << numberString(plan.fare, 2)
        << ",\"fareStrategy\":\"" << jsonEscape(plan.fareStrategy) << "\""
        << ",\"passengerType\":\"" << jsonEscape(passengerTypeToString(plan.passengerType)) << "\""
        << ",\"availableSeats\":" << bus.getAvailableSeats()
        << ",\"capacity\":" << bus.getCapacity()
        << ",\"stops\":[";

    for (size_t i = 0; i < plan.route.stopIds.size(); ++i) {
        if (i > 0) out << ',';
        const RouteStop& stop = graph.getStop(plan.route.stopIds[i]);
        string tag = (i == 0) ? "START"
                             : (i + 1 == plan.route.stopIds.size() ? "END" : "STOP");
        out << "{\"id\":" << stop.getId()
            << ",\"name\":\"" << jsonEscape(stop.getName()) << "\""
            << ",\"tag\":\"" << tag << "\"}";
    }

    out << "],\"legs\":[";
    for (size_t i = 0; i < plan.route.legs.size(); ++i) {
        if (i > 0) out << ',';
        const Road& leg = plan.route.legs[i];
        int fromId = plan.route.stopIds[i];
        int toId = plan.route.stopIds[i + 1];
        out << "{\"from\":" << fromId
            << ",\"to\":" << toId
            << ",\"fromName\":\"" << jsonEscape(graph.getStop(fromId).getName()) << "\""
            << ",\"toName\":\"" << jsonEscape(graph.getStop(toId).getName()) << "\""
            << ",\"line\":\"" << jsonEscape(leg.lineName) << "\""
            << ",\"distance\":" << numberString(leg.distanceKm, 1)
            << ",\"speed\":" << numberString(leg.speedLimitKmh, 0) << "}";
    }
    out << "]}}";
    return out.str();
}

static string buildCompareJson(const BusNetworkManager& manager, int startId,
                               int destId, int modeChoice, int hour,
                               PassengerType passengerType) {
    const BusGraph& graph = manager.getGraph();
    const vector<unique_ptr<Bus>>& fleet = manager.getFleet();

    if (!graph.hasStop(startId)) {
        throw InvalidStopException("Origin stop ID " + to_string(startId) +
                                   " does not exist.");
    }
    if (!graph.hasStop(destId)) {
        throw InvalidStopException("Destination stop ID " + to_string(destId) +
                                   " does not exist.");
    }
    if (startId == destId) {
        throw InvalidInputException("Origin and destination must be different.");
    }

    OptimizationMode mode = parseOptimizationMode(modeChoice);
    float rush = BusNetworkManager::rushMultiplierForHour(hour);

    ostringstream out;
    out << "{\"ok\":true,\"compare\":{";
    out << "\"mode\":\"" << jsonEscape(optimizationModeToString(mode)) << "\""
        << ",\"hour\":" << hour
        << ",\"rush\":" << numberString(rush, 2)
        << ",\"passengerType\":\"" << jsonEscape(passengerTypeToString(passengerType)) << "\""
        << ",\"options\":[";

    bool first = true;
    for (const auto& busPtr : fleet) {
        const Bus& bus = *busPtr;
        RouteResult route = graph.findRoute(startId, destId, mode, bus, rush);
        if (!first) out << ',';
        first = false;

        out << "{\"bus\":\"" << jsonEscape(bus.getType()) << "\""
            << ",\"busId\":\"" << jsonEscape(bus.getId()) << "\"";
        if (!route.found) {
            out << ",\"found\":false}";
            continue;
        }

        unique_ptr<FareStrategy> strategy =
            FareStrategyFactory::create(passengerType, bus.isExpressBus());
        float fare = strategy->calculate(
            FareContext{route.totalDistanceKm, bus.getFareRate(),
                        bus.getBoardingFee()});

        out << ",\"found\":true"
            << ",\"distance\":" << numberString(route.totalDistanceKm, 1)
            << ",\"minutes\":" << numberString(route.totalMinutes, 1)
            << ",\"transfers\":" << route.transfers
            << ",\"fare\":" << numberString(fare, 2)
            << ",\"strategy\":\"" << jsonEscape(strategy->name()) << "\"}";
    }
    out << "]}}";
    return out.str();
}

static string buildBookJson(const JourneyPlan& plan, const Ticket& ticket,
                            const BusNetworkManager& manager) {
    const Bus& bus = *manager.getFleet().at(static_cast<size_t>(plan.busIndex));

    ostringstream out;
    out << "{\"ok\":true,\"ticket\":{";
    out << "\"id\":\"" << jsonEscape(ticket.getId()) << "\""
        << ",\"passenger\":\"" << jsonEscape(ticket.getPassenger().getName()) << "\""
        << ",\"passengerType\":\"" << jsonEscape(passengerTypeToString(plan.passengerType)) << "\""
        << ",\"bus\":\"" << jsonEscape(bus.getType() + " [" + bus.getId() + "]") << "\""
        << ",\"route\":\"" << jsonEscape(plan.originName + " -> " + plan.destinationName) << "\""
        << ",\"distance\":" << numberString(plan.route.totalDistanceKm, 2)
        << ",\"minutes\":" << numberString(plan.route.totalMinutes, 1)
        << ",\"fare\":" << numberString(plan.fare, 2)
        << ",\"fareStrategy\":\"" << jsonEscape(plan.fareStrategy) << "\""
        << ",\"availableSeats\":" << bus.getAvailableSeats()
        << "}}";
    return out.str();
}

static string handleApiRequest(BusNetworkManager& manager, const string& request) {
    string action = jsonGetRaw(request, "action");

    if (action == "network") {
        return buildNetworkJson(manager);
    }

    if (action == "plan") {
        int start = jsonGetInt(request, "start", -1);
        int dest = jsonGetInt(request, "dest", -1);
        int mode = jsonGetInt(request, "mode", 1);
        int hour = jsonGetInt(request, "hour", 9);
        int passenger = jsonGetInt(request, "passenger", 1);
        int bus = jsonGetInt(request, "bus", 1);
        PassengerType passengerType = parsePassengerType(passenger);

        JourneyPlan plan;
        {
            ScopedCoutSilencer silence;
            plan = manager.planJourney(start, dest, mode, hour, bus, passengerType);
        }
        return buildPlanJson(plan, manager, mode, hour);
    }

    if (action == "compare") {
        int start = jsonGetInt(request, "start", -1);
        int dest = jsonGetInt(request, "dest", -1);
        int mode = jsonGetInt(request, "mode", 1);
        int hour = jsonGetInt(request, "hour", 9);
        int passenger = jsonGetInt(request, "passenger", 1);
        PassengerType passengerType = parsePassengerType(passenger);
        return buildCompareJson(manager, start, dest, mode, hour, passengerType);
    }

    if (action == "book") {
        int start = jsonGetInt(request, "start", -1);
        int dest = jsonGetInt(request, "dest", -1);
        int mode = jsonGetInt(request, "mode", 1);
        int hour = jsonGetInt(request, "hour", 9);
        int passenger = jsonGetInt(request, "passenger", 1);
        int bus = jsonGetInt(request, "bus", 1);
        string name = jsonGetRaw(request, "name");
        if (trimCopy(name).empty()) name = "Passenger";
        PassengerType passengerType = parsePassengerType(passenger);

        string payload;
        {
            ScopedCoutSilencer silence;
            JourneyPlan plan = manager.planJourney(start, dest, mode, hour,
                                                   bus, passengerType);
            Ticket ticket = manager.bookTicket(plan, name);
            payload = buildBookJson(plan, ticket, manager);
        }
        return payload;
    }

    throw InvalidInputException("Unknown API action: '" + action + "'.");
}

static int runApiMode() {
    try {
        BusNetworkManager manager;
        string line;
        while (getline(cin, line)) {
            if (trimCopy(line).empty()) continue;
            try {
                cout << handleApiRequest(manager, line) << "\n" << flush;
            } catch (const exception& ex) {
                cout << "{\"ok\":false,\"error\":\"" << jsonEscape(ex.what())
                     << "\"}\n" << flush;
            } catch (...) {
                cout << "{\"ok\":false,\"error\":\"Unknown API error.\"}\n" << flush;
            }
        }
    } catch (const exception& ex) {
        cout << "{\"ok\":false,\"error\":\"" << jsonEscape(ex.what())
             << "\"}\n" << flush;
        return 1;
    }
    return 0;
}

// ==========================================
// MAIN INTERACTION MENU
// ==========================================
int main(int argc, char* argv[]) {
    if (getenv("NO_COLOR") != nullptr) Style::enabled = false;
    bool runTests = false;
    bool runApi = false;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--no-color") Style::enabled = false;
        if (arg == "--test") runTests = true;
        if (arg == "--api") runApi = true;
    }

    if (runApi) return runApiMode();
    if (runTests) {
        runSelfTests();
        return 0;
    }

    try {
        // [FIXED] manager construction is now inside try
        BusNetworkManager manager;
        int choice = 0;

        while (choice != 7) {
            printTitle("SMART BUS ROUTE PLANNER");
            cout << "  " << Style::paint("Pune City Network  |  Dijkstra Routing  |  OOP Demo",
                                         Style::DIM) << "\n";
            printRule();
            printMenuItem(1, "View Route Network & Fleet");
            printMenuItem(2, "Plan Journey (Dijkstra + Fare + Booking)");
            printMenuItem(3, "Compare All Bus Types");
            printMenuItem(4, "Cancel Ticket");
            printMenuItem(5, "Trip History & Statistics");
            printMenuItem(6, "System Statistics");
            printMenuItem(7, "Exit System");
            printRule();

            if (!askNumber("Enter your choice: ", choice)) {
                if (cin.eof()) {
                    cout << "\n  " << Style::paint("Input stream closed. Exiting.",
                                                    Style::YELLOW) << "\n";
                    break;
                }
                cout << "  " << Style::paint("Invalid input. Enter a number.",
                                              Style::YELLOW) << "\n";
                continue;
            }

            try {
                switch (choice) {
                    case 1:
                        manager.displayNetwork();
                        manager.displayFleet();
                        break;

                    case 2: {
                        manager.displayNetwork();
                        manager.displayFleet();

                        int startId = 0, destId = 0, modeChoice = 0;
                        int hour = 0, busChoice = 0, passengerChoice = 0;

                        if (!askNumber("Enter Origin Stop ID: ", startId)) {
                            throw InvalidInputException("Origin stop ID must be numeric.");
                        }
                        if (!askNumber("Enter Destination Stop ID: ", destId)) {
                            throw InvalidInputException("Destination stop ID must be numeric.");
                        }
                        if (!askNumber("Mode (1 Fastest, 2 Shortest, 3 Fewest Transfers): ",
                                       modeChoice)) {
                            throw InvalidInputException("Optimization mode must be numeric.");
                        }
                        if (!askNumber("Time of Day (0-23, rush applied automatically): ",
                                       hour)) {
                            throw InvalidInputException("Hour must be numeric.");
                        }
                        if (!askNumber("Passenger Type (1 Regular, 2 Student, 3 Senior): ",
                                       passengerChoice)) {
                            throw InvalidInputException("Passenger type must be numeric.");
                        }
                        PassengerType passengerType;
                        try {
                            passengerType = parsePassengerType(passengerChoice);
                        } catch (...) {
                            throw InvalidInputException("Passenger type must be 1, 2, or 3.");
                        }
                        if (!askNumber("Fleet Type (1-4): ", busChoice)) {
                            throw InvalidInputException("Fleet choice must be numeric.");
                        }

                        JourneyPlan plan = manager.planJourney(
                            startId, destId, modeChoice, hour, busChoice,
                            passengerType);

                        int bookingChoice = 0;
                        if (!askNumber("Book a seat? (1 = Yes, 0 = No): ", bookingChoice)) {
                            throw InvalidInputException("Booking choice must be numeric.");
                        }
                        if (bookingChoice == 1) {
                            string passengerName =
                                readLineValue("Passenger name: ");
                            if (passengerName.empty()) {
                                throw InvalidInputException(
                                    "Passenger name cannot be empty.");
                            }
                            Ticket ticket = manager.bookTicket(plan, passengerName);
                            cout << "\n  " << Style::paint("BOOKING CONFIRMED",
                                                            Style::BOLD + Style::GREEN)
                                 << "\n";
                            cout << ticket;
                        } else {
                            cout << "\n  " << Style::paint("Booking skipped.",
                                                            Style::DIM) << "\n";
                        }
                        break;
                    }

                    case 3: {
                        int startId = 0, destId = 0, modeChoice = 0;
                        int hour = 0, passengerChoice = 0;

                        if (!askNumber("Enter Origin Stop ID: ", startId)) {
                            throw InvalidInputException("Origin stop ID must be numeric.");
                        }
                        if (!askNumber("Enter Destination Stop ID: ", destId)) {
                            throw InvalidInputException("Destination stop ID must be numeric.");
                        }
                        if (!askNumber("Mode (1 Fastest, 2 Shortest, 3 Fewest Transfers): ",
                                       modeChoice)) {
                            throw InvalidInputException("Optimization mode must be numeric.");
                        }
                        if (!askNumber("Time of Day (0-23): ", hour)) {
                            throw InvalidInputException("Hour must be numeric.");
                        }
                        if (!askNumber("Passenger Type (1 Regular, 2 Student, 3 Senior): ",
                                       passengerChoice)) {
                            throw InvalidInputException("Passenger type must be numeric.");
                        }
                        PassengerType passengerType;
                        try {
                            passengerType = parsePassengerType(passengerChoice);
                        } catch (...) {
                            throw InvalidInputException("Passenger type must be 1, 2, or 3.");
                        }

                        manager.compareFleet(startId, destId, modeChoice, hour,
                                             passengerType);
                        break;
                    }

                    case 4: {
                        string ticketId = readLineValue("Enter Ticket ID to cancel: ");
                        if (ticketId.empty()) {
                            throw InvalidInputException("Ticket ID cannot be empty.");
                        }
                        manager.cancelTicket(ticketId);
                        break;
                    }

                    case 5:
                        manager.showTripHistory();
                        break;

                    case 6:
                        manager.showSystemStats();
                        break;

                    case 7:
                        cout << "\n  " << Style::paint("Exiting system. Safe travels!",
                                                        Style::BOLD + Style::GREEN) << "\n";
                        break;

                    default:
                        cout << "  " << Style::paint("Invalid choice. Enter 1-7.",
                                                      Style::YELLOW) << "\n";
                }
            } catch (const BusFullException& ex) {
                cout << "\n  " << Style::paint("[BOOKING FAILED] " + string(ex.what()),
                                                Style::RED) << "\n";
            } catch (const InvalidInputException& ex) {
                cout << "\n  " << Style::paint("[WARNING] " + string(ex.what()),
                                                Style::YELLOW) << "\n";
            } catch (const InvalidStopException& ex) {
                cout << "\n  " << Style::paint("[STOP ERROR] " + string(ex.what()),
                                                Style::RED) << "\n";
            } catch (const RouteNotFoundException& ex) {
                cout << "\n  " << Style::paint("[SYSTEM ERROR] " + string(ex.what()),
                                                Style::RED) << "\n";
            } catch (const exception& ex) {
                cout << "\n  " << Style::paint("[UNEXPECTED ERROR] " + string(ex.what()),
                                                Style::RED) << "\n";
            }

            if (cin.eof()) {
                cout << "\n  " << Style::paint("Input stream closed. Exiting.",
                                                Style::YELLOW) << "\n";
                break;
            }
        }
    } catch (const exception& ex) {
        cerr << "\n[FATAL STARTUP ERROR] " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
