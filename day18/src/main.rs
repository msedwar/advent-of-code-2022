use std::collections::{HashSet, VecDeque};
use std::io::prelude::*;

#[derive(Clone, Copy, Debug, Eq, Hash, PartialEq)]
struct Vector32 {
    x: i32,
    y: i32,
    z: i32,
}

impl Vector32 {
    pub fn zero() -> Vector32 {
        Vector32 {
            x: 0,
            y: 0,
            z: 0,
        }
    }

    pub fn add(&self, x: i32, y: i32, z: i32) -> Vector32 {
        Vector32 {
            x: self.x + x,
            y: self.y + y,
            z: self.z + z,
        }
    }

    pub fn sub(&self, other: &Vector32) -> Vector32 {
        Vector32 {
            x: self.x - other.x,
            y: self.y - other.y,
            z: self.z - other.z,
        }
    }
}

#[derive(Eq, Hash, PartialEq)]
struct CubeFace {
    cube: Vector32,
    face: Vector32,
}

fn uncovered_sides(cube: &Vector32, cube_map: &HashSet<Vector32>) -> usize {
    let mut sides: usize = 0;
    if !cube_map.contains(&cube.add(-1, 0, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.add(1, 0, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.add(0, -1, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.add(0, 1, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.add(0, 0, -1)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.add(0, 0, 1)) {
        sides += 1;
    }
    sides
}

fn get_part1_surface_area(cube_map: &HashSet<Vector32>) -> usize {
    let mut surface_area: usize = 0;
    for cube in cube_map.iter() {
        surface_area += uncovered_sides(cube, &cube_map);
    }
    surface_area
}

fn get_bounds(cube_map: &HashSet<Vector32>) -> (Vector32, Vector32) {
    let mut min_bounds = Vector32::zero();
    let mut max_bounds = Vector32::zero();
    for cube in cube_map.iter() {
        if cube.x < min_bounds.x {
            min_bounds.x = cube.x;
        }
        if cube.x > max_bounds.x {
            max_bounds.x = cube.x;
        }
        if cube.y < min_bounds.y {
            min_bounds.y = cube.y;
        }
        if cube.y > max_bounds.y {
            max_bounds.y = cube.y;
        }
        if cube.z < min_bounds.z {
            min_bounds.z = cube.z;
        }
        if cube.z > max_bounds.z {
            max_bounds.z = cube.z;
        }
    }
    min_bounds.x -= 1;
    min_bounds.y -= 1;
    min_bounds.z -= 1;
    max_bounds.x += 1;
    max_bounds.y += 1;
    max_bounds.z += 1;
    (min_bounds, max_bounds)
}

fn within_bounds(cube: &Vector32, min_bounds: &Vector32, max_bounds: &Vector32) -> bool {
    return cube.x >= min_bounds.x
        && cube.x <= max_bounds.x
        && cube.y >= min_bounds.y
        && cube.y <= max_bounds.y
        && cube.z >= min_bounds.z
        && cube.z <= max_bounds.z;
}

fn get_part2_surface_area(cube_map: &HashSet<Vector32>) -> usize {
    let (min_bounds, max_bounds) = get_bounds(cube_map);
    assert!(min_bounds != max_bounds);
    let mut to_visit: VecDeque<Vector32> = VecDeque::new();
    let mut visited: HashSet<Vector32> = HashSet::new();
    let mut lit_faces: HashSet<CubeFace> = HashSet::new();
    to_visit.push_back(min_bounds);

    while !to_visit.is_empty() {
        let current = to_visit.pop_front().unwrap();
        if visited.contains(&current) {
            continue;
        }
        visited.insert(current);
        let neighbors = [
            current.add(-1, 0, 0),
            current.add(1, 0, 0),
            current.add(0, -1, 0),
            current.add(0, 1, 0),
            current.add(0, 0, -1),
            current.add(0, 0, 1),
        ];
        for neighbor in neighbors {
            if cube_map.contains(&neighbor) {
                lit_faces.insert(CubeFace{
                    cube: neighbor,
                    face: current.sub(&neighbor),
                });
            } else if !visited.contains(&neighbor) && within_bounds(&neighbor, &min_bounds, &max_bounds) {
                to_visit.push_back(neighbor);
            }
        }
    }
    lit_faces.len()
}

fn parse_cubes(cubes: &mut HashSet<Vector32>) {
    for line in std::io::stdin().lock().lines() {
        match line {
            Ok(l) => {
                let coords = l.split(",").collect::<Vec<&str>>();
                assert!(coords.len() == 3);
                let cube = Vector32{
                    x: coords[0].parse::<i32>().unwrap(),
                    y: coords[1].parse::<i32>().unwrap(),
                    z: coords[2].parse::<i32>().unwrap(),
                };
                cubes.insert(cube);
            },
            _ => panic!("Could not parse input"),
        }
    }
}

fn main() {
    let mut cube_map: HashSet<Vector32> = HashSet::new();
    parse_cubes(&mut cube_map);
    println!("Part 1 surface area: {}", get_part1_surface_area(&cube_map));
    println!("Part 2 surface area: {}", get_part2_surface_area(&cube_map));
}
