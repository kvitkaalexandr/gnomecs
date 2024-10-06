# GnomeCS

Yet another ECS written in C.

## Disclaimer

This is my hobby pet-project, so it's not production-ready and may contain critical bugs. Use it at your own risk.

This project didn't aim to be the better or the fastest ECS in the world. It's just a simple hobby project created for educational and fun purposes.

Current state: Work in progress, not finished yet. API may change. 

## Key features

### Custom memory allocator with relative pointers

World data are stored in a single memory block.
All pointers inside are relative, it is just an integer offset in bytes from the beginning of the memory block.
This allows to copy the entire game state with a simple `memcpy`.
For example, it can be useful for multiplayer games with rollback mechanism.

### Static archetypes

GnomeCS is archetype-based ECS. Static means that you can't add or remove components from the entity after its creation.

### Zero dependencies

C11, standard library only, nothing else.

## How it looks

Define components:
```c
_component(positionComponent, {
    float x;
    float y;
    float z;
});

_componentTag(playerTag);
```

Create world:
```c
gWorld *world = gCreateWorld();

_useWorld(world);
_addComponentToWorld(positionComponent);
_addComponentToWorld(playerTag);
```

Create entity:
```c
_createEntity(playerEntity, 
    _componentId(positionComponent), 
    _componentId(playerTag)
);
```

Make a query:
```c
_makeQuery
    _with(positionComponent)
    _with(playerTag)
_foreach {
    positionComponent *p = _take(positionComponent);
    printf("Player position: %f %f %f\n", p->x, p->y, p->z);
}
```

More examples can be found in the example repository (TODO: add link).

## Fun fact

Right now you have an entity in the form of a gnome.
