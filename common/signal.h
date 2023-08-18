#pragma once
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <thread>
#include <type_traits>
#include <variant>
#include <vector>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

//===============================================
template <typename T, std::enable_if_t<!std::is_void_v<T>, bool> = true>
struct all_values {
    typedef std::vector<std::decay_t<T>> result_type;

    template <typename InputIter>
    result_type operator()(InputIter first, InputIter last) const {
        result_type res;
        while (first != last) {
            // res.emplace_back(*first);
            try {
                res.emplace_back(*first);
            } catch (const std::exception &e) {
                // std::cout << e.what() << std::endl;
                throw std::runtime_error(e.what());
            };
            ++first;
        }
        return res;
    }
};
//===============================================
template <typename T>
struct last_value {
    typedef std::optional<T> result_type;

    template <typename InputIter>
    result_type operator()(InputIter first, InputIter last) const {
        std::optional<T> value;
        while (first != last) {
            value = *first;
            ++first;
        }
        return value;
    }
};
template <>
struct last_value<void> {
    typedef void result_type;

    template <typename InputIter>
    void operator()(InputIter first, InputIter last) const {
        while (first != last) {
            *first;
            ++first;
        }
    }
};

class SlotBase;
class SignalBase {
    friend class SlotBase;

public:
    virtual ~SignalBase() {}

protected:
    virtual std::shared_ptr<void> lock_pimpl() const = 0;
};

using tracked_container_type = std::vector<std::variant<std::weak_ptr<void>>>;
using locked_container_type = std::vector<std::variant<std::shared_ptr<void>>>;

class SlotBase {
protected:
    tracked_container_type _trackedObjects;
    locked_container_type _lockedObjects;

public:
    virtual ~SlotBase() {}

    SlotBase() = default;

    const tracked_container_type &tracked_objects() const { return _trackedObjects; }

    // if one of tracked objects is expired, return ture, otherwise return false
    bool expired() const {
        for (auto &&e : _trackedObjects) {
            if (std::visit(overloaded{[](const std::weak_ptr<void> &arg) { return arg.expired(); }}, e)) return true;
        }
        return false;
    }

    //
    locked_container_type lock() const {
        locked_container_type ret;
        for (auto &&e : _trackedObjects) {
            ret.emplace_back(std::visit(overloaded{[](const std::weak_ptr<void> &arg) {
                                            if (arg.expired()) throw std::runtime_error("expired slot");
                                            return arg.lock();
                                        }},
                                        e));
        }
        return ret;
    }

protected:
    void track_signal(const SignalBase &signal) { _trackedObjects.emplace_back(signal.lock_pimpl()); }
};

template <typename _Signature>
class Slot;

template <typename Ret_, typename... Args>
    requires std::invocable<Ret_(Args...), Args...>
class Slot<Ret_(Args...)> : public SlotBase {
public:
    using signature_type = Ret_(Args...);
    using function_type = std::function<signature_type>;
    using result_type = Ret_;

    // template <typename T>
    Slot() = default;

    template <typename Function, typename T, typename ReturnType = std::invoke_result_t<Function, T *, Args...>>
        requires std::invocable<Function, T *, Args...> && std::is_same_v<ReturnType, Ret_>
    Slot(Function &&f, T *owner) {
        _slotFunction = [func = std::move(f), owner](Args... args) -> Ret_ {
            if constexpr (std::is_void_v<ReturnType>)
                std::invoke(func, owner, args...);
            else
                return std::invoke(func, owner, args...);
        };
    }

    template <typename Function, typename ReturnType = std::invoke_result_t<Function, Args...>>
        requires std::invocable<Function, Args...> && std::is_same_v<ReturnType, Ret_>
    Slot(Function &&f) {
        // #if __cpp_lib_move_only_function
        _slotFunction = [func = std::move(f)](Args... args) -> Ret_ {
            if constexpr (std::is_void_v<ReturnType>)
                std::invoke(func, args...);
            else
                std::invoke(func, args...);
        };
    }

    // check if empty
    explicit operator bool() const noexcept { return bool(_slotFunction); }

    // invocation
    result_type operator()(Args... args) const {
        lock();
        if (_slotFunction) return _slotFunction(args...);
    }

    Slot &Track(const std::weak_ptr<void> &v) {
        _trackedObjects.emplace_back(v);
        return *this;
    }
    Slot &Track(const SignalBase &v) {
        track_signal(v);
        return *this;
    }
    Slot &Track(const SlotBase &v) {
        for (auto &&e : v.tracked_objects()) _trackedObjects.emplace_back(e);
        return *this;
    }

    // slot function access
    constexpr const function_type &SlotFunction() const { return _slotFunction; }
    constexpr function_type &SlotFunction() { return _slotFunction; }

    bool operator==(const Slot &other) const {
        return _slotFunction.target<signature_type>() == other._slotFunction.target<signature_type>();
    }

private:
    function_type _slotFunction;
};

// called by combiner, wrap slot iter and arguments
template <typename _ConnectionBody, typename _ConnectionBodyIter, typename... _Args>
class SlotCallIterator {
public:
    using result_type = typename _ConnectionBody::slot_type::result_type;

    SlotCallIterator(_ConnectionBodyIter it, std::shared_ptr<std::tuple<_Args...>> args) : _iter(it), _args(args) {}
    result_type operator*() { return invokeFunc(std::make_index_sequence<sizeof...(_Args)>{}); }
    bool operator==(const SlotCallIterator &other) { return _iter == other._iter; }
    bool operator!=(const SlotCallIterator &other) { return _iter != other._iter; }
    SlotCallIterator &operator++() {
        ++_iter;
        return *this;
    };
    SlotCallIterator &operator--() {
        --_iter;
        return *this;
    };
    SlotCallIterator operator++(int) {
        auto ret = *this;
        _iter++;
        return ret;
    };
    SlotCallIterator operator--(int) {
        auto ret = *this;
        _iter++;
        return ret;
    };

private:
    _ConnectionBodyIter _iter;
    std::shared_ptr<std::tuple<_Args...>> _args;

    template <size_t... I>
    result_type invokeFunc(std::index_sequence<I...>) {
        return (*(*_iter)->GetSlot())(std::get<I>(*_args)...);
    }
};

class ConnectionBodyBase {
    mutable bool _connected{true};

public:
    virtual ~ConnectionBodyBase() {}

    void Disconnect() {
        if (_connected) {
            std::unique_lock<ConnectionBodyBase> lk(*this);
            ReleaseSlot();
            _connected = false;
        }
    }
    bool Connected() const {
        if (SlotExpired()) _connected = false;
        return _connected;
    }

    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual std::shared_ptr<void> ReleaseSlot() = 0;
    virtual bool SlotExpired() const = 0;
};

template <typename _SlotType, typename _GroupKey>
class ConnectionBody : public ConnectionBodyBase {
    _GroupKey _group{};
    std::shared_ptr<_SlotType> _slot;
    std::shared_ptr<std::mutex> _mutex;  // from  signal

public:
    using slot_type = _SlotType;

    ConnectionBody(const _SlotType &slotIn, const std::shared_ptr<std::mutex> &signalMutex) noexcept
        : _slot(new _SlotType(slotIn)), _mutex(signalMutex) {}
    ConnectionBody(_GroupKey group, const _SlotType &slotIn, const std::shared_ptr<std::mutex> &signalMutex) noexcept
        : _group(group), _slot(new _SlotType(slotIn)), _mutex(signalMutex) {}
    ~ConnectionBody() { Disconnect(); }

    void DisconnectExpiredSlot() {
        if (_slot && _slot->Expired()) {
            Disconnect();
        }
    }

    void lock() override { _mutex->lock(); }
    void unlock() override { _mutex->unlock(); }

    constexpr std::shared_ptr<_SlotType> GetSlot() const { return _slot; }

    std::shared_ptr<void> ReleaseSlot() override {
        std::shared_ptr<_SlotType> ret;
        ret.swap(_slot);
        return ret;
    }
    bool SlotExpired() const override { return _slot->expired(); }
    const _GroupKey &GroupKey() const { return _group; }
    void SetGroupKey(const _GroupKey &group) { _group = group; }
};

class Connection {
    std::weak_ptr<ConnectionBodyBase> _weakConnectionBody;

public:
    Connection(const std::weak_ptr<ConnectionBodyBase> &connectionBody) noexcept
        : _weakConnectionBody(connectionBody) {}

    // connection management
    void Disconnect() const {
        if (!_weakConnectionBody.expired()) _weakConnectionBody.lock()->Disconnect();
    }
    bool Connected() const noexcept {
        if (!_weakConnectionBody.expired()) return _weakConnectionBody.lock()->Connected();
        return false;
    }

    void Swap(Connection &other) noexcept { std::swap(_weakConnectionBody, other._weakConnectionBody); }

    // blocking
    // bool Blocked() const noexcept {}
    // bool operator==(const Connection &other) const noexcept {}
    // bool operator!=(const Connection &other) const noexcept { return !(*this == other); }
    // bool operator<(const Connection &other) const noexcept {}
};

template <typename _Signature, typename _Combiner = last_value<typename std::function<_Signature>::result_type>,
          typename _Group = int, std::enable_if_t<std::is_function_v<_Signature>, bool> = true>
class Signal : public SignalBase {
public:
    using signature_type = _Signature;
    using group_type = _Group;
    using slot_function_type = std::function<signature_type>;
    using slot_type = Slot<signature_type>;
    using combiner_type = _Combiner;
    using result_type = typename combiner_type::result_type;
    using connection_body_type = ConnectionBody<slot_type, group_type>;

    struct ConnectionCompare {
        bool operator()(const std::shared_ptr<connection_body_type> &lhs,
                        const std::shared_ptr<connection_body_type> &rhs) const {
            return lhs->GroupKey() < rhs->GroupKey();
        };
    };
    using connection_container_type = std::multiset<std::shared_ptr<connection_body_type>, ConnectionCompare>;
    using connection_container_iterator_type = typename connection_container_type::iterator;

    Signal() : _mutex(std::make_shared<std::mutex>()), _handle(std::make_shared<int>()) {}
    ~Signal() {}

    template <typename Function, typename T>
    Connection Connect(Function &&f, T *owner, group_type group = 0) {
        const std::lock_guard<std::mutex> lock(*_mutex);
        return Connection(
            *_connections.emplace(std::make_shared<connection_body_type>(group, slot_type(f, owner), _mutex)));
    }
    template <typename Function>
    Connection Connect(Function &&f, group_type group = 0) {
        const std::lock_guard<std::mutex> lock(*_mutex);
        return Connection(*_connections.emplace(std::make_shared<connection_body_type>(group, slot_type(f), _mutex)));
    }

    bool Contains(const slot_type &slot) {
        return std::find_if(_connections.cbegin(), _connections.cend(),
                            [&slot](auto &&e) { return *e->GetSlot() == slot; }) != _connections.cend();
    }
    template <typename Function, typename T>
    void Disconnect(Function &&f, T *owner) {
        auto it = std::find_if(_connections.cbegin(), _connections.cend(),
                               [slot = slot_type(f, owner)](auto &&e) { return *e->GetSlot() == slot; });
        if (it != _connections.cend()) {
            _connections.erase(it);
            return;
        }
    }
    template <typename Function>
    void Disconnect(Function &&f) {
        ;
        auto it = std::find_if(_connections.cbegin(), _connections.cend(),
                               [slot = slot_type(f)](auto &&e) { return *e->GetSlot() == slot; });
        if (it != _connections.cend()) {
            _connections.erase(it);
            return;
        }
    }
    void DisconnectAllConnections() { _connections.clear(); }
    constexpr bool Empty() const { return _connections.empty(); }
    constexpr size_t ConnectionNum() const { return _connections.size(); }

    void ClearExpiredConnections() const {
        while (true) {
            auto it = std::find_if(_connections.begin(), _connections.end(), [](auto &&e) {
                auto a = e->GetSlot();
                return !a || e->SlotExpired();
            });
            if (it == _connections.end())
                break;
            else
                _connections.erase(it);
        }
    };

    template <typename... Args>
    result_type operator()(Args &&...args) const {
        ClearExpiredConnections();
        auto a = std::make_shared<std::tuple<Args...>>(args...);
        return _combiner(
            SlotCallIterator<connection_body_type, connection_container_iterator_type, Args...>(_connections.begin(),
                                                                                                a),
            SlotCallIterator<connection_body_type, connection_container_iterator_type, Args...>(_connections.end(), a));
    }

    constexpr std::shared_ptr<int> getHandle() const { return _handle; }

private:
    mutable connection_container_type _connections{};

    std::shared_ptr<std::mutex> _mutex;
    std::shared_ptr<int> _handle;

    combiner_type _combiner{};

    std::shared_ptr<void> lock_pimpl() const override { return _handle; }
};
