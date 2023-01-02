use std::io::prelude::*;

use regex::Regex;

const TOTAL_MINUTES: usize = 24;

struct Blueprint {
    id: u32,
    ore_robot_ore_cost: u32,
    clay_robot_ore_cost: u32,
    obsidian_robot_ore_cost: u32,
    obsidian_robot_clay_cost: u32,
    geode_robot_ore_cost: u32,
    geode_robot_obsidian_cost: u32,
}

#[derive(Clone, Copy)]
struct State<'a> {
    blueprint: &'a Blueprint,
    round: usize,
    ore: usize,
    clay: usize,
    obsidian: usize,
    geodes: usize,
    ore_robots: usize,
    clay_robots: usize,
    obsidian_robots: usize,
    geode_robots: usize,
}

fn simulate_round<'a>(state: &'a State) -> State<'a> {
    let mut round_state = state.clone();
    round_state.round += 1;
    round_state.ore += state.ore_robots;
    round_state.clay += state.clay_robots;
    round_state.obsidian += state.obsidian_robots;
    round_state.geodes += state.geode_robots;
    return round_state;
}

fn get_max_for_round(state: &State) -> usize {
    let round_state = simulate_round(state);
    if round_state.round == TOTAL_MINUTES {
        return round_state.geodes;
    }
    0
}

fn get_max_for_blueprint(blueprint: &Blueprint) -> usize {
    let initial_state = State{
        blueprint: blueprint,
        round: 0,
        ore: 0,
        clay: 0,
        obsidian: 0,
        geodes: 0,
        ore_robots: 1,
        clay_robots: 0,
        obsidian_robots: 0,
        geode_robots: 0,
    };
    get_max_for_round(&initial_state)
}

fn parse_blueprints(blueprints: &mut Vec<Blueprint>) {
    let re = Regex::new(r"(?x)
    ((?-x)Blueprint (?P<id>\d+): )
    ((?-x)Each ore robot costs (?P<ore_ore>\d+) ore. )
    ((?-x)Each clay robot costs (?P<clay_ore>\d+) ore. )
    ((?-x)Each obsidian robot costs (?P<obs_ore>\d+) ore and (?P<obs_clay>\d+) clay. )
    ((?-x)Each geode robot costs (?P<geo_ore>\d+) ore and (?P<geo_obs>\d+) obsidian.)
    ").unwrap();

    for line in std::io::stdin().lock().lines() {
        match line {
            Ok(l) => {
                if let Some(costs) = re.captures(&l) {
                    blueprints.push(Blueprint{
                        id: costs["id"].parse::<u32>().unwrap(),
                        ore_robot_ore_cost: costs["ore_ore"].parse::<u32>().unwrap(),
                        clay_robot_ore_cost: costs["clay_ore"].parse::<u32>().unwrap(),
                        obsidian_robot_ore_cost: costs["obs_ore"].parse::<u32>().unwrap(),
                        obsidian_robot_clay_cost: costs["obs_clay"].parse::<u32>().unwrap(),
                        geode_robot_ore_cost: costs["geo_ore"].parse::<u32>().unwrap(),
                        geode_robot_obsidian_cost: costs["geo_obs"].parse::<u32>().unwrap(),
                    });
                } else {
                    panic!("Could not parse line");
                }
            },
            _ => panic!("Could not parse input"),
        }
    }
}

fn main() {
    let mut blueprints: Vec<Blueprint> = Vec::new();
    parse_blueprints(&mut blueprints);

    let mut total_quality: usize = 0;
    for blueprint in blueprints {
        let geodes = get_max_for_blueprint(&blueprint);
        println!("Blueprint {} produces {} geodes", blueprint.id, geodes);
        total_quality += geodes * (blueprint.id as usize);
    }
    println!("Total quality level: {}", total_quality);
}
