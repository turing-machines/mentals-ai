#pragma once

#include "core.h"


template <typename T>
class Buffer {
public:
    virtual ~Buffer() = default;

    virtual void set_data(const T& data) = 0;
    virtual T get_data() const = 0;
    virtual void append(const T& data) = 0;
    virtual void push_back(const typename T::value_type& data) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
};


class StringBuffer : public Buffer<std::string> {
public:
    StringBuffer() : buffer("") {}

    void set_data(const std::string& str) override { buffer = str; }
    std::string get_data() const override { return buffer; }
    void append(const std::string& str) override { buffer += str; }
    void push_back(const char& c) override { buffer += c; }
    void clear() override { buffer.clear(); }
    size_t size() const override { return buffer.size(); }

private:
    std::string buffer;

};


template <typename T>
class SafeChunkBuffer : public Buffer<std::deque<T>> {
public:
    SafeChunkBuffer() = default;

    void set_data(const std::deque<T>& chunks) override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        chunk_buffer = chunks;
    }

    std::deque<T> get_data() const override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        return chunk_buffer;
    }

    void append(const std::deque<T>& chunks) override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        chunk_buffer.insert(chunk_buffer.end(), chunks.begin(), chunks.end());
    }

    void push_back(const typename std::deque<T>::value_type& chunk) override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        chunk_buffer.emplace_back(chunk);
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        chunk_buffer.clear();
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        return chunk_buffer.size();
    }

    T& back() {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        return chunk_buffer.back();
    }

    expected<T*, std::string> get_chunk(const std::function<bool(const T&)>& handler) {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        for (auto& chunk : chunk_buffer) {
            if (handler(chunk)) { return &chunk; }
        }
        return unexpected<std::string>("Chunk not found");
    }

    expected<void, std::string> remove_chunk(const std::function<bool(const T&)>& handler) {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        auto it = std::find_if(chunk_buffer.begin(), chunk_buffer.end(), handler);
        if (it != chunk_buffer.end()) {
            chunk_buffer.erase(it);
            return {};
        }
        return unexpected<std::string>("Chunk not found");
    }

    size_t get_chunks_count() const {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        return chunk_buffer.size();
    }

    std::vector<T> get_all_chunks() const {
        std::lock_guard<std::mutex> lock(chunk_buffer_mutex);
        return std::vector<T>(chunk_buffer.begin(), chunk_buffer.end());
    }

private:
    std::deque<T> chunk_buffer;
    mutable std::mutex chunk_buffer_mutex;

};
