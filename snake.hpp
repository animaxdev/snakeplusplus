#ifndef SNAKEHPPINCLUDED
#define SNAKEHPPINCLUDED

#include <algorithm>
#include <list>

#include <point2d.hpp>
#include <direction.hpp>
#include <location_source.hpp>
#include <fruit_manager.hpp>

//class Screen;

class Snake {
public:
  Snake(FruitManager *fruit_manager, LocationSource *location_source,
        const Point2D<int> &pos, Direction dir) :
    fruit_manager_{fruit_manager}, location_source_{location_source},
    head_position_{pos}, direction_{dir}, last_moved_direction_{dir}
    { location_source_->take(head_position_); }

  Point2D<int> head_position() const { return head_position_; }
  int length() const { return body_locations_.size() + 1; }
  int occupies(const Point2D<int> &pos) const {
    return ((pos == head_position()) || body_occupies(pos));
  }

  void move();
  void face(Direction dir);

  //void draw(Screen *screen);

private:
  FruitManager *fruit_manager_;
  LocationSource *location_source_;
  Point2D<int> head_position_;
  Direction direction_;
  Direction last_moved_direction_;
  std::list<Point2D<int>> body_locations_;

  void grow_body();
  void move_body();
  bool body_occupies(const Point2D<int> &pos) const {
    return (std::find(body_locations_.begin(),
                      body_locations_.end(),
                      pos) != body_locations_.end());
  }
};

#endif