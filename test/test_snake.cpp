#include <exception>
#include <stdexcept>

#include <vector>

#include <catch.hpp>

#include <location_source.hpp>
#include <fruit_manager.hpp>
#include <snake.hpp>
#include <dummyrenderapi.hpp>


TEST_CASE( "Create a snake", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {3, 3};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  REQUIRE( snake.occupies({0, 0}) );
}

TEST_CASE( "Snake movements", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {3, 3};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  SECTION( "Basic movement" )
  {
    snake.move();

    REQUIRE( snake.occupies({1, 0}) );

    snake.move();

    REQUIRE( snake.occupies({2, 0}) );
  }
  SECTION( "Change a snake's direction" )
  {
    snake.face(Direction::down);
    snake.move();

    REQUIRE( snake.occupies({0, 1}) );

    snake.face(Direction::right);
    snake.move();

    REQUIRE( snake.occupies({1, 1}) );
  }
  SECTION( "A snake can't go in the direction it last moved in" )
  {
    snake.face(Direction::left);
    snake.move();

    REQUIRE( snake.occupies({1, 0}) );

    snake.face(Direction::left);
    snake.move();

    REQUIRE( snake.occupies({2, 0}) );

    snake.face(Direction::down);
    snake.move();

    REQUIRE( snake.occupies({2, 1}) );

    snake.face(Direction::up);
    snake.move();

    REQUIRE( snake.occupies({2, 2}) );
  }
}

TEST_CASE( "A snake shouldn't be able to go outside the grid", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {3, 3};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  SECTION( "A snake can't go outside the grid in the positive direction" )
  {
    snake.move();
    snake.move();

    REQUIRE_THROWS_AS( snake.move(), std::runtime_error );
  }
  SECTION( "A snake can't go outside the grid in the negative direction" )
  {
    snake.face(Direction::up);

    REQUIRE_THROWS_AS( snake.move(), std::runtime_error );
  }
}

TEST_CASE( "A snake can grow as it moves", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {3, 3};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  fruit_manager.place_fruit({1, 0});
  location_source.add_location({1, 1});
  location_source.add_location({0, 1});
  location_source.add_location({2, 2});

  SECTION( "A snake can eat a fruit to grow" )
  {
    snake.move();

    REQUIRE( snake.length() == 2 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({1, 0}) );
  }
  SECTION( "A snake that has grown takes its body with it" )
  {
    snake.move();

    REQUIRE( snake.length() == 2 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({1, 0}) );

    snake.move();

    REQUIRE( snake.length() == 2 );
    REQUIRE_FALSE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({2, 0}) );
  }
  SECTION( "A snake can chase its own tail safely" )
  {
    snake.move();

    REQUIRE( snake.length() == 2 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({1, 0}) );

    snake.face(Direction::down);
    snake.move();

    REQUIRE( snake.length() == 3 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({1, 1}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({1, 1}) );

    snake.face(Direction::left);
    snake.move();

    REQUIRE( snake.length() == 4 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({1, 1}) );
    REQUIRE( snake.occupies({0, 1}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({0, 1}) );

    snake.face(Direction::up);
    snake.move();

    REQUIRE( snake.length() == 4 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({1, 1}) );
    REQUIRE( snake.occupies({0, 1}) );

    snake.face(Direction::right);
    snake.move();

    REQUIRE( snake.length() == 4 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({1, 1}) );
    REQUIRE( snake.occupies({0, 1}) );
  }
}

TEST_CASE( "A snake dies if it eats its own body", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {3, 3};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  fruit_manager.place_fruit({1, 0});
  location_source.add_location({2, 0});
  location_source.add_location({2, 1});
  location_source.add_location({1, 1});

  SECTION( "A snake dies if it eats its own body" )
  {
    snake.move();

    REQUIRE( snake.length() == 2 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );

    REQUIRE( fruit_manager.fruit_at({2, 0}) );
    REQUIRE_FALSE( fruit_manager.fruit_at({1, 0}) );

    snake.move();

    REQUIRE( snake.length() == 3 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({2, 0}) );

    REQUIRE( fruit_manager.fruit_at({2, 1}) );
    REQUIRE_FALSE( fruit_manager.fruit_at({2, 0}) );

    snake.face(Direction::down);
    snake.move();

    REQUIRE( snake.length() == 4 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({2, 0}) );
    REQUIRE( snake.occupies({2, 1}) );

    REQUIRE( fruit_manager.fruit_at({1, 1}) );
    REQUIRE_FALSE( fruit_manager.fruit_at({2, 1}) );

    snake.face(Direction::left);
    snake.move();

    REQUIRE( snake.length() == 5 );
    REQUIRE( snake.occupies({0, 0}) );
    REQUIRE( snake.occupies({1, 0}) );
    REQUIRE( snake.occupies({2, 0}) );
    REQUIRE( snake.occupies({2, 1}) );
    REQUIRE( snake.occupies({1, 1}) );

    REQUIRE_FALSE( fruit_manager.fruit_at({1, 1}) );

    snake.face(Direction::up);

    REQUIRE_THROWS_AS( snake.move(), std::runtime_error );
  }
}

TEST_CASE( "An error is raised when there are no locations left", "[snake]" )
{
  DummyRendererFactory renderer {};
  TestLocationSource location_source {2, 2};
  FruitManager fruit_manager {&location_source, &renderer};
  Snake snake {&fruit_manager, &location_source, {0, 0}, Direction::right,
               &renderer};

  fruit_manager.place_fruit({1, 0});
  location_source.add_location({1, 1});
  location_source.add_location({0, 1});

  SECTION( "An error is raised when there are no locations left" )
  {
    snake.move();
    REQUIRE( snake.length() == 2 );

    snake.face(Direction::down);
    snake.move();
    REQUIRE( snake.length() == 3 );

    snake.face(Direction::left);
    REQUIRE_THROWS_AS( snake.move(), no_available_locations_error);
  }
}
