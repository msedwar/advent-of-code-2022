#![feature(test)]
extern crate test;

use std::collections::VecDeque;

const TOTAL_ROCKS: usize = 1000000000000; //1000000000000;
const BOARD_WIDTH: usize = 7;

const HLINE_BOX: [(usize, usize); 4] = [(0, 0), (1, 0), (2, 0), (3, 0)];
const CROSS_BOX: [(usize, usize); 5] = [(1, 0), (0, 1), (1, 1), (2, 1), (1, 2)];
const REVERSEL_BOX: [(usize, usize); 5] = [(0, 0), (1, 0), (2, 0), (2, 1), (2, 2)];
const VLINE_BOX: [(usize, usize); 4] = [(0, 0), (0, 1), (0, 2), (0, 3)];
const SQUARE_BOX: [(usize, usize); 4] = [(0, 0), (1, 0), (0, 1), (1, 1)];

#[derive(Clone, Copy, Debug)]
enum Jet {
    Left,
    Right,
}

#[derive(Clone, Copy, Debug)]
#[repr(usize)]
#[allow(dead_code)]
enum Rock {
    HLine = 0,
    Cross = 1,
    ReverseL = 2,
    VLine = 3,
    Square = 4,
}

impl Rock {
    pub fn get_width(&self) -> usize {
        match self {
            Rock::HLine => 4,
            Rock::Cross => 3,
            Rock::ReverseL => 3,
            Rock::VLine => 1,
            Rock::Square => 2,
        }
    }

    pub fn get_height(&self) -> usize {
        match self {
            Rock::HLine => 1,
            Rock::Cross => 3,
            Rock::ReverseL => 3,
            Rock::VLine => 4,
            Rock::Square => 2,
        }
    }

    pub fn get_collision_box(&self) -> &'static [(usize, usize)] {
        match self {
            Rock::HLine => &HLINE_BOX,
            Rock::Cross => &CROSS_BOX,
            Rock::ReverseL => &REVERSEL_BOX,
            Rock::VLine => &VLINE_BOX,
            Rock::Square => &SQUARE_BOX,
        }
    }
}

#[derive(Debug)]
struct Board<'a> {
    rocks: VecDeque<bool>,
    offset: usize,
    height: usize,
    highest_rock: usize,
    jet_index: usize,
    jets: &'a [Jet],
}

impl<'a> Board<'a> {
    pub fn new(jets: &'a [Jet]) -> Self {
        Board {
            rocks: VecDeque::new(),
            offset: 0,
            height: 0,
            highest_rock: 0,
            jet_index: 0,
            jets: jets,
        }
    }

    pub fn get_highest_block(&self) -> usize {
        self.highest_rock
    }

    pub fn drop_next_rock(&mut self, index: usize) {
        if index % 10000000 == 0 {
            println!("Progress: {:.5}%", (index as f64) / (TOTAL_ROCKS as f64) * 100f64)
        }

        if self.height < self.highest_rock + 4 {
            self.extend_rows(self.highest_rock + 1000);
        }

        let rock: Rock = unsafe { std::mem::transmute(index % 5) };
        let mut rock_x: usize = 2;
        let mut rock_y: usize = self.highest_rock + 3;

        loop {
            self.push_rock(rock, &mut rock_x, rock_y);
            if self.drop_rock(rock, rock_x, &mut rock_y) {
                return;
            }
        }
    }

    #[inline(always)]
    fn push_rock(&mut self, rock: Rock, rock_x: &mut usize, rock_y: usize) {
        match self.get_next_jet() {
            Jet::Left => {
                if *rock_x > 0 && !self.collides(rock, *rock_x - 1, rock_y) {
                    *rock_x -= 1;
                }
            }
            Jet::Right => {
                if *rock_x + rock.get_width() < BOARD_WIDTH
                    && !self.collides(rock, *rock_x + 1, rock_y)
                {
                    *rock_x += 1;
                }
            }
        }
    }

    #[inline(always)]
    fn drop_rock(&mut self, rock: Rock, rock_x: usize, rock_y: &mut usize) -> bool {
        if *rock_y > 0 && !self.collides(rock, rock_x, *rock_y - 1) {
            *rock_y -= 1;
            return false;
        } else {
            self.freeze(rock, rock_x, *rock_y);
            return true;
        }
    }

    fn collides(&self, rock: Rock, rx: usize, ry: usize) -> bool {
        if rx + rock.get_width() - 1 >= BOARD_WIDTH {
            return true;
        }
        for (x, y) in rock.get_collision_box() {
            let bx = x + rx;
            let by = y + ry;
            if by < self.height && self.get_cell_at(bx, by) {
                return true;
            }
        }
        return false;
    }

    fn freeze(&mut self, rock: Rock, rx: usize, ry: usize) {
        for (x, y) in rock.get_collision_box() {
            let bx = x + rx;
            let by = y + ry;
            *self.get_ref_cell_at(bx, by) = true;
        }
        let height_last = rock.get_height() + ry;
        if height_last > self.highest_rock {
            self.highest_rock = height_last;
        }
    }

    #[inline(always)]
    fn get_next_jet(&mut self) -> Jet {
        let ret = self.jets[self.jet_index % self.jets.len()];
        self.jet_index += 1;
        ret
    }

    #[inline(always)]
    fn get_cell_at(&self, x: usize, y: usize) -> bool {
        self.rocks[(y - self.offset) * BOARD_WIDTH + x]
    }

    #[inline(always)]
    fn get_ref_cell_at(&mut self, x: usize, y: usize) -> &mut bool {
        &mut self.rocks[(y - self.offset) * BOARD_WIDTH + x]
    }

    fn extend_rows(&mut self, new_height: usize) {
        // Row elision.
        if self.highest_rock - self.offset > 2000 {
            let row = self.get_highest_complete_row();
            if row > self.offset + 1 {
                self.rocks.drain(0..(row - 1 - self.offset) * BOARD_WIDTH);
                self.offset = row - 1;
                debug_assert!(self.rocks.len() == (self.height - self.offset) * BOARD_WIDTH);
            }
        }

        // Rock extension.
        let num_cells: usize = (new_height - self.height) * BOARD_WIDTH;
        self.rocks.extend(std::iter::repeat(false).take(num_cells));
        self.height = new_height;
    }

    fn get_highest_complete_row(&self) -> usize {
        let mut min: usize = self.height - 1;

        for x in 0..BOARD_WIDTH {
            for y in (self.offset..self.height).rev() {
                if self.get_cell_at(x, y) {
                    min = std::cmp::min(min, y);
                    break;
                }
            }
        }
        return min;
    }
}

impl std::fmt::Display for Board<'_> {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for y in (0..self.height).rev() {
            write!(f, "|")?;
            for x in 0..BOARD_WIDTH {
                write!(
                    f,
                    "{}",
                    if self.get_cell_at(x, y) {
                        "#"
                    } else {
                        "."
                    }
                )?;
            }
            write!(f, "|\n")?;
        }
        write!(f, "{:-<1$}", "", (BOARD_WIDTH + 2).try_into().unwrap())?;
        Ok(())
    }
}

fn parse_jets(jets: &mut Vec<Jet>) {
    let mut buffer = String::new();
    match std::io::stdin().read_line(&mut buffer) {
        Ok(_) => (),
        _ => panic!("Could not parse input"),
    }

    for c in buffer.chars() {
        match c {
            '<' => jets.push(Jet::Left),
            '>' => jets.push(Jet::Right),
            _ => panic!("Invalid input: {}", c),
        }
    }
}

fn main() {
    let mut jets: Vec<Jet> = Vec::new();
    parse_jets(&mut jets);
    let mut board: Board = Board::new(&jets);

    for round in 0..TOTAL_ROCKS {
        board.drop_next_rock(round);
    }

    println!("Highest rock at {}", board.get_highest_block());
}

#[cfg(test)]
mod tests {
    use super::*;
    use test::Bencher;

    #[bench]
    fn bench_1000(b: &mut Bencher) {
        let jets: Vec<Jet> = vec![Jet::Left, Jet::Left, Jet::Right, Jet::Left, Jet::Right];
        
        b.iter(|| {
            let mut board: Board = Board::new(&jets);
            for round in 0..1000 {
                board.drop_next_rock(round);
            }
        });
    }

    #[bench]
    fn bench_1(b: &mut Bencher) {
        let jets: Vec<Jet> = vec![Jet::Left, Jet::Left, Jet::Right, Jet::Left, Jet::Right];
        let mut board: Board = Board::new(&jets);
        let mut round: usize = 0;
        
        b.iter(|| {
            board.drop_next_rock(round);
            round += 1;
        });
    }
}
