const TOTAL_ROCKS: usize = 2022;
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
    rocks: Vec<bool>,
    height: usize,
    jet_index: usize,
    jets: &'a [Jet],
}

impl<'a> Board<'a> {
    pub fn new(jets: &'a [Jet]) -> Self {
        Board {
            rocks: Vec::new(),
            height: 0,
            jet_index: 0,
            jets: jets,
        }
    }

    pub fn drop_rock(&mut self, index: usize) {
        let highest_rock = self.get_heighest_rock();

        if self.height < highest_rock + 4 {
            let num_cells: usize = (highest_rock + 4 - self.height) * BOARD_WIDTH;
            self.rocks.extend(std::iter::repeat(false).take(num_cells));
            self.height = highest_rock + 4;
        }

        let rock: Rock = unsafe { std::mem::transmute(index % 5) };
        let mut rock_x: usize = 2;
        let mut rock_y: usize = self.height - 1;
        assert!(!self.collides(rock, rock_x, rock_y));

        loop {
            // Push rock.
            match self.get_next_jet() {
                Jet::Left => {
                    if rock_x > 0 && !self.collides(rock, rock_x - 1, rock_y) {
                        rock_x -= 1;
                    }
                }
                Jet::Right => {
                    if rock_x + rock.get_width() < BOARD_WIDTH
                        && !self.collides(rock, rock_x + 1, rock_y)
                    {
                        rock_x += 1;
                    }
                }
            }

            // Drop rock.
            if rock_y > 0 && !self.collides(rock, rock_x, rock_y - 1) {
                rock_y -= 1;
            } else {
                self.freeze(rock, rock_x, rock_y);
                break;
            }
        }
    }

    fn collides(&self, rock: Rock, rx: usize, ry: usize) -> bool {
        for (x, y) in rock.get_collision_box() {
            let bx = x + rx;
            let by = y + ry;
            if bx >= BOARD_WIDTH || (by < self.height && self.rocks[by * BOARD_WIDTH + bx]) {
                return true;
            }
        }
        return false;
    }

    fn freeze(&mut self, rock: Rock, rx: usize, ry: usize) {
        for (x, y) in rock.get_collision_box() {
            let bx = x + rx;
            let by = y + ry;
            self.rocks[by * BOARD_WIDTH + bx] = true;
        }
    }

    fn get_next_jet(&mut self) -> Jet {
        let ret = self.jets[self.jet_index % self.jets.len()];
        self.jet_index += 1;
        ret
    }

    pub fn get_heighest_rock(&self) -> usize {
        for y in (0..self.height).rev() {
            for x in 0..BOARD_WIDTH {
                if self.rocks[y * BOARD_WIDTH + x] {
                    return y + 1;
                }
            }
        }
        return 0;
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
                    if self.rocks[y * BOARD_WIDTH + x] {
                        "#"
                    } else {
                        " "
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
        board.drop_rock(round);
    }

    println!("Highest rock at {}", board.get_heighest_rock());
}
