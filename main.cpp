#include <unistd.h>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class CProduct {
  static unsigned int MAX_SIZE;

 public:
  bool empty() { return m_products.empty(); }
  bool full() { return m_products.size() >= MAX_SIZE; }
  std::mutex m_mutex;
  std::queue<T> m_products;
};
template <typename T>
unsigned int CProduct<T>::MAX_SIZE = 10;

class CPCModel {
 public:
  CPCModel() {}
  void produce() {
    std::unique_lock<std::mutex> lock(m_product.m_mutex);
    m_product.m_products.push("product 1");
    std::cout << "produce product: " <<  m_product.m_products.back() << std::endl;
    m_cv.notify_one();
  }

  void consume() {
    std::unique_lock<std::mutex> lock(m_product.m_mutex);
    m_cv.wait(lock, [this] { return !m_product.empty(); });
    std::cout << "consume product: " << m_product.m_products.front()
              << std::endl;
    m_product.m_products.pop();
  }

 protected:
  std::condition_variable m_cv;
  CProduct<std::string> m_product;
};

static void worker_producer(CPCModel& model) { model.produce(); }
static void worker_consumer(CPCModel& model) { model.consume(); }

int main(int argc, char* argv[]) {
  CPCModel model;
  std::thread producer(worker_producer, std::ref(model));
  std::thread consumer(worker_consumer, std::ref(model));
  producer.join();
  consumer.join();
  return 0;
}