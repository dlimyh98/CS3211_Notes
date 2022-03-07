#include <cassert>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

struct Token {
    std::string secret;
};

class TokenPromise {
    Token token;

    std::atomic<bool> isValid;

public:
    TokenPromise() : token{}, isValid{ false } {}

    // May only be called once
    void publishToken(Token t) {
        assert(!isValid);
        token = t;

        // Release fence followed by atomic store has the same effect as a
        // store release, even if the store is relaxed
        std::atomic_thread_fence(std::memory_order_release);
        isValid.store(true, std::memory_order_relaxed);
    }

    /* RELEASE fence FA in thread A synchronizes with ACQUIRE fence FB in thread B
    *  1. exists an atomic object M (isValid)
    *  2. exists an atomic WRITE X (with any memory order), that modifies M in thread A (isValid.store)
    *  3. FA is sequenced before X in thread A
    *  4. exists an atomic READ Y (with any memory order) in thread B (iValid.load)
    *  5. Y is sequenced before FB in thread B
    * 
    *  - all non-atomic and relaxed atomic stores sequenced before FA in thread A HAPPENS-BEFORE
    *    all non-atomic and relaxed atomic stores from THE SAME LOCATIONS made in thread B AFTER FB
    */

    // No limits on who or how many times this can be called
    std::optional<Token> get_token() {
        // Atomic load followed by acquire fence has the same effect as a load
        // acquire, even if the load is relaxed
        if (isValid.load(std::memory_order_relaxed)) {
            std::atomic_thread_fence(std::memory_order_acquire);
            return token;
        }
        else {
            return std::nullopt;
        }
    }
};

TokenPromise luminus_password{};

int main() {
    std::thread get_password{ []() {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      luminus_password.publishToken(Token{"abcdef"});
    } };

    std::thread use_password{ []() {
      while (true) {
        std::optional<Token> t = luminus_password.get_token();
        if (t) {
          std::cout << t->secret << std::endl;
          break;
        }
      }
    } };

    get_password.join();
    use_password.join();

    return 0;
}