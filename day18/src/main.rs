use std::collections::HashSet;
use std::io::prelude::*;

#[derive(Eq, Hash, PartialEq)]
struct Cube {
    x: i32,
    y: i32,
    z: i32,
}

impl Cube {
    pub fn translate(&self, x: i32, y: i32, z: i32) -> Cube {
        Cube {
            x: self.x + x,
            y: self.y + y,
            z: self.z + z,
        }
    }
}

fn uncovered_sides(cube: &Cube, cube_map: &HashSet<Cube>) -> usize {
    let mut sides: usize = 0;
    if !cube_map.contains(&cube.translate(-1, 0, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.translate(1, 0, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.translate(0, -1, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.translate(0, 1, 0)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.translate(0, 0, -1)) {
        sides += 1;
    }
    if !cube_map.contains(&cube.translate(0, 0, 1)) {
        sides += 1;
    }
    sides
}

fn parse_cubes(cubes: &mut HashSet<Cube>) {
    for line in std::io::stdin().lock().lines() {
        match line {
            Ok(l) => {
                let coords = l.split(",").collect::<Vec<&str>>();
                assert!(coords.len() == 3);
                let cube = Cube{
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
    let mut cube_map: HashSet<Cube> = HashSet::new();
    parse_cubes(&mut cube_map);

    let mut surface_area: usize = 0;
    for cube in cube_map.iter() {
        surface_area += uncovered_sides(cube, &cube_map);
    }

    println!("Surface area: {}", surface_area);
}
