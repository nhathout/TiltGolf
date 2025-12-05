# ----------------------------------------------------
# Box2D Source Integration (Explicit List)
# ----------------------------------------------------

INCLUDEPATH += $$PWD/Box2D/include $$PWD/Box2D/src
CONFIG += c++11

# --- Dynamics ---
SOURCES += \
    $$PWD/Box2D/src/dynamics/b2_body.cpp \
    $$PWD/Box2D/src/dynamics/b2_chain_circle_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_chain_polygon_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_circle_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_contact_manager.cpp \
    $$PWD/Box2D/src/dynamics/b2_contact_solver.cpp \
    $$PWD/Box2D/src/dynamics/b2_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_distance_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_edge_circle_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_edge_polygon_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_fixture.cpp \
    $$PWD/Box2D/src/dynamics/b2_friction_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_gear_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_island.cpp \
    $$PWD/Box2D/src/dynamics/b2_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_motor_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_mouse_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_polygon_circle_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_polygon_contact.cpp \
    $$PWD/Box2D/src/dynamics/b2_prismatic_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_pulley_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_revolute_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_weld_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_wheel_joint.cpp \
    $$PWD/Box2D/src/dynamics/b2_world.cpp \
    $$PWD/Box2D/src/dynamics/b2_world_callbacks.cpp

# --- Collision ---
SOURCES += \
    $$PWD/Box2D/src/collision/b2_broad_phase.cpp \
    $$PWD/Box2D/src/collision/b2_chain_shape.cpp \
    $$PWD/Box2D/src/collision/b2_circle_shape.cpp \
    $$PWD/Box2D/src/collision/b2_collide_circle.cpp \
    $$PWD/Box2D/src/collision/b2_collide_edge.cpp \
    $$PWD/Box2D/src/collision/b2_collide_polygon.cpp \
    $$PWD/Box2D/src/collision/b2_collision.cpp \
    $$PWD/Box2D/src/collision/b2_distance.cpp \
    $$PWD/Box2D/src/collision/b2_dynamic_tree.cpp \
    $$PWD/Box2D/src/collision/b2_edge_shape.cpp \
    $$PWD/Box2D/src/collision/b2_polygon_shape.cpp \
    $$PWD/Box2D/src/collision/b2_time_of_impact.cpp

# --- Common ---
SOURCES += \
    $$PWD/Box2D/src/common/b2_block_allocator.cpp \
    $$PWD/Box2D/src/common/b2_draw.cpp \
    $$PWD/Box2D/src/common/b2_math.cpp \
    $$PWD/Box2D/src/common/b2_settings.cpp \
    $$PWD/Box2D/src/common/b2_stack_allocator.cpp \
    $$PWD/Box2D/src/common/b2_timer.cpp

# --- Rope ---
SOURCES += \
    $$PWD/Box2D/src/rope/b2_rope.cpp
