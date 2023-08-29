use std::fs;
use crate::pattern::Pattern;
use crate::repertoire::Repertoire;
use crate::board::Board;
use crate::timer::Timer;
use colored::Colorize;

const LETTTERS_VALUES : [u16; 26] = [1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 10, 1, 2, 1, 1, 3, 8, 1, 1, 1, 1, 4, 10, 10, 10, 10];

enum Bonuses {
    Word(usize),
    Letter(usize),
    Start(usize),
    None,
}

const BOARD_BONUS : [[Bonuses; 15] ; 15] =
[
    [Bonuses::Word(3), Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::Word(3)],
    [Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None],
    [Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None],
    [Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::Letter(2)],
    [Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None],
    [Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None],
    [Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None],

    [Bonuses::Word(3), Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Start(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::Word(3)],

    [Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None],
    [Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None],
    [Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::None],
    [Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::Letter(2)],
    [Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None],
    [Bonuses::None, Bonuses::Word(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(2), Bonuses::None],
    [Bonuses::Word(3), Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Word(3), Bonuses::None, Bonuses::None, Bonuses::None, Bonuses::Letter(2), Bonuses::None, Bonuses::None, Bonuses::Word(3)],

];

#[derive(Clone, Copy)]
pub struct Letter {
    letter : char,
    value : u16,
}

impl std::fmt::Display for Letter {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.letter)
    }
}

#[derive(Debug)]
struct LetterAndRange {
    start: usize,
    end: usize,
    letter: String,
}

#[derive(Debug)]
struct StartEnd {
    start: usize,
    end: usize,
}

#[derive(Debug)]
struct Word {
    word: String,
    point: usize,
}

#[derive(Debug)]
struct Output {
    word: Word,
    start: usize,
    row: usize,
    is_collumn: bool,
}

pub struct Scrabble {
    board : Board::<Letter>,
    rotated_board : Board::<Letter>,
}



fn sub_pattern(groups: &Vec<LetterAndRange>, spaces: &Vec<usize>, start: usize, size: usize) -> Pattern {
    let start_offset: i32 = match start {
        0 => 0,
        _ => -1
    };

    let mut pattern = Pattern::with_capacity(2 + size * 10);
    pattern.push((spaces[start] as i32 + start_offset) as u8, &groups[start].letter);

    for i in 1..size {
        pattern.push(spaces[i + start] as u8, &groups[i + start].letter);
    }

    let last_index = size + start;
    let offset : i32;
    if last_index == spaces.len() - 1 {
        offset = 0;
    } else {
        offset = -1;
    }
    pattern.push((spaces[last_index] as i32 + offset) as u8, "");

    return pattern;

}

fn build_pattern(groups: &Vec<LetterAndRange>, spaces: &Vec<usize>) -> Vec<Pattern> {
    let n = groups.len();

    let mut patterns = Vec::with_capacity(n * (n + 1) / 2);

    for i in (1..=n).rev() {
        for j in 0..=n - i {
            patterns.push(sub_pattern(groups, spaces, j, i));
        }
    }

    return patterns;
}

fn build_letters(groups: &Vec<LetterAndRange>) -> Vec<String> {
    let mut ret = vec![String::new(); groups.len()];
    for i in 0..ret.len() {
        ret[i] = groups[i].letter.clone();
    }

    return ret;
}

fn convert_row(row: &[Letter]) -> u32 {
    let mut ret = 0;
    for (i, c) in row.iter().enumerate() {
        if c.letter != '#' {
            ret |= 1 << i;
        }
    }
    return ret;
}

fn convert_row_padded(row: &[Letter]) -> u32 {
    let mut ret: u32 = 0;

    if row[1].letter != '#' || row[0].letter != '#' {
        ret |= 1;
    }

    for i in 1..row.len() - 1 {
        if row[i + 1].letter != '#' || row[i - 1].letter != '#' || row[i].letter != '#' {
            ret |= 1 << i;
        }
    }

    if row[row.len() - 2].letter != '#' || row.last().unwrap().letter != '#' {
        ret |= 1 << (row.len() - 1);
    }

    return ret;
}

fn get_ancor_help(_ancor: u32, row_size: usize) -> Vec<StartEnd> {
    let mut ancors = vec![];

    let mut start: i32 = -1;
    for i in 0..row_size {
        if _ancor & ( 1 << i) != 0 {
            if start == -1 {
                start = i as i32;
            }
        } else if start != -1 {
            ancors.push(StartEnd {start: start as usize, end: i - 1});
            start = -1;
        }
    }

    if start != -1 {
        ancors.push(StartEnd { start: start as usize, end: row_size - 1 });
    }

    return ancors;
}

fn get_ancor_edge(row: &[Letter], next_row: &[Letter]) -> Vec<StartEnd> {
        let row_padded_bin = convert_row_padded(row);
        let next_row_bin = convert_row(next_row);

        let ancors = !row_padded_bin & next_row_bin;

        return get_ancor_help(ancors, row.len());
}

fn get_ancor(row: &[Letter], previus_row: &[Letter], next_row: &[Letter]) -> Vec<StartEnd> {
    let row_padded_bin: u32 = convert_row_padded(row);
    let previus_row_bin = convert_row(previus_row);
    let next_row_bin = convert_row(next_row);
    let ancors = !row_padded_bin & (next_row_bin | previus_row_bin);

    return get_ancor_help(ancors, row.len());
}

fn build_spaces(groups: &Vec<LetterAndRange>, row_size: usize) -> Vec<usize> {
    let mut spaces = vec![0; groups.len() + 1];

    let mut start = 0;

    for (i, group) in groups.into_iter().enumerate() {
        spaces[i] = group.start - start;
        start = group.end + 1;
    }

    *spaces.last_mut().unwrap() = row_size - start;
    return spaces;
}

fn build_groups(row: &[Letter]) -> Vec<LetterAndRange> {
    let mut ret: Vec<LetterAndRange> = vec![];
    let mut buff = LetterAndRange {
        start: 0,
        end: row.len() - 1,
        letter: String::new()
    };

    for (i, c) in row.iter().enumerate() {
        if c.letter != '#' {
            if buff.letter.is_empty() {
                buff.start = i;
            }
            buff.letter.push(c.letter);
        } else if !buff.letter.is_empty() {
            buff.end = i - 1;
            ret.push(buff);
            buff = LetterAndRange{start: 0, end: row.len() - 1, letter: String::new()};
        }
    }

    if !buff.letter.is_empty() {
        ret.push(buff);
    }

    return ret;
}

fn get_word_helper(row: &[Letter], start: usize, insert_index: usize, letter: Letter) -> Word
{
    let mut test_word = String::with_capacity(row.len());
    let mut point = 0;

    for i in start..row.len() {
        if row[i].letter != '#' {
            test_word.push(row[i].letter);
            point += row[i].value as usize;
        } else if i == insert_index {
            test_word.push(letter.letter);
            point += letter.value as usize;
        } else {
            break;
        }

    }

    return Word { word: test_word, point };
}

fn get_word(row: &[Letter], row_index: usize, letter: Letter) -> Word {
    if row_index == 0 {
        return get_word_helper(row, 0, row_index, letter);
    }

    for i in (0..=row_index - 1).rev() {
        if row[i].letter == '#' {
            return get_word_helper(row, i + 1, row_index, letter);
        }
    }

    return get_word_helper(row, 0, row_index, letter);
}

fn get_value(c: char) -> u16 {
    return LETTTERS_VALUES[c as usize - 'A' as usize];
}

fn get_point_helper(row: &[Letter], rotated_board: &Board<Letter>, word: &str, start_of_word_in_row: usize, row_index: usize, repertoire: &Repertoire) -> Option<usize> {
    let mut letter_placed = 0;
    let mut other_points = 0;
    let mut word_mul = 1;
    let mut word_point = 0;

    let mut chars = word.chars();


    for i in start_of_word_in_row..word.len() + start_of_word_in_row {
        let c = chars.next().unwrap();
        if row[i].letter == '#' {
            let mut current_letter = Letter {letter: c, value: get_value(c)};
            match BOARD_BONUS[row_index][i] {
                Bonuses::Word(b) => word_mul *= b,
                Bonuses::Start(b) => word_mul *= b,
                Bonuses::Letter(b) => current_letter.value *= b as u16,
                _ => (),
            }

            let test_word = get_word(&rotated_board[i], row_index, current_letter);

            if !repertoire.valide(&test_word.word) {
                return None;
            }

            letter_placed += 1;
            if test_word.word.len() > 1 {
                other_points += test_word.point;
            }
            word_point += current_letter.value as usize;
        } else {
            word_point += row[i].value as usize;
        }
    }

    if letter_placed == 7 {
        other_points += 50;
    }

    return Some(word_mul * word_point + other_points);
}

struct Jsp {
    point: usize,
    start: usize,
}

fn get_point(row: &[Letter], rotated_board: &Board<Letter>, groups: &Vec<LetterAndRange>, case: usize, location: usize, word: &str, row_index: usize, repertoire: &Repertoire) -> Option<Jsp> {
    let index;
    {
        let mut i = 0;
        let mut case = case;
        while i <= case {
            case -= i;
            i += 1;
        }
        index = case;
    }

    let start_of_word_in_row = groups[index].start - location;

    if let Some(point) =  get_point_helper(row, rotated_board, word, start_of_word_in_row, row_index, repertoire) {
        return Some(Jsp { point, start: start_of_word_in_row });
    }
    return None;
}

fn find_best_word(letters: &str, joker_count: usize, repertoire: &Repertoire, regexes: &Vec<Pattern>, letters_on_board: &Vec<String>, board: &Board<Letter>, rotated_board: &Board<Letter>, groups: &Vec<LetterAndRange>, row_index: usize, is_collumn: bool, out: &mut Vec<Output>) {
    let canditates = repertoire.advance_filter(letters, joker_count, regexes, letters_on_board);
    for i in 0..canditates.words.len() {
        for location in &canditates.regex_resultes[i].locations {
            if let Some(point) = get_point(&board[row_index], rotated_board, groups, canditates.regex_resultes[i].case, *location, &repertoire[canditates.words[i]], row_index, repertoire) {
                out.push(Output { word: Word { word: repertoire[canditates.words[i]].clone(), point: point.point }, start: point.start, row: row_index, is_collumn});
            }
        }
    }
}

fn overlap(a: StartEnd, b: &StartEnd) -> bool {
    if a.start >= b.start && a.start <= b.end {
        return true;
    }

    if a.end >= b.start && a.end <= b.end {
        return true;
    }

    if a.start >= b.start && a.end >= b.end {
        return true;
    }

    return false;
}

fn containe(a: StartEnd, b: &StartEnd) -> bool {
    if a.start <= b.start && a.end >= b.end {
        return true;
    }
    return false;
}

fn find_best_word_ancors_helper(row: &[Letter], rotated_board: &Board<Letter>, start: usize, end: usize, ancor: &StartEnd, word: &str, row_index: usize, is_collumn: bool, repertoire: &Repertoire, out: &mut Vec<Output>) {
    for i in start..=ancor.end {
        if i + word.len() > end {
            return;
        }

        if overlap(StartEnd {start: i, end: i + word.len() - 1}, ancor) {
            if let Some(point) = get_point_helper(row, rotated_board, word, i, row_index, repertoire) {
                out.push(Output { word: Word { word: String::from(word), point }, start: i, row: row_index, is_collumn});
            }
        }
    }
}

fn find_best_word_ancors(row: &[Letter], rotated_board: &Board<Letter>, groups: &Vec<LetterAndRange>, ancors: &Vec<StartEnd>, words: &Vec<&String>, row_index: usize, is_collumn: bool, repertoire: &Repertoire, out: &mut Vec<Output>) {
    let mut start = 0;

    let mut should_add_tow = false;
    if let Some(group) = groups.first() {
        if group.end == 0 {
            should_add_tow = true;
        }
    }

    'ancors: for ancor in ancors {
        for group in groups {
            if start != 0 {
                should_add_tow = true;
            }

            let test = StartEnd { start, end: group.start };
            if containe(test, ancor) {
                for i in 0..words.len() {
                    find_best_word_ancors_helper(row, rotated_board, if should_add_tow {start + 2} else { start }, group.start - 1, ancor, words[i], row_index, is_collumn, repertoire, out);
                }
                start = group.end;
                continue 'ancors;
            }
            start = group.end;
        }

        if containe(StartEnd { start, end: row.len() - 1 }, ancor) {
            for i in 0..words.len() {
                find_best_word_ancors_helper(row, rotated_board, start + 2, row.len(), ancor, words[i], row_index, is_collumn, repertoire, out);
            }
        }
    }
}

fn get_color(letter: char, j: usize, i: usize) -> colored::ColoredString  {
    let bonus = &BOARD_BONUS[j][i];
    match bonus {
        Bonuses::Word(v) => {
            if *v == 2 {
                return format!("{letter}").yellow();
            }
            return format!("{letter}").red();
        },
        Bonuses::Letter(v) => {
            if *v == 2 {
                return format!("{letter}").cyan();
            }
            return String::from(letter).blue();
        }

        Bonuses::Start(_) => {
            return format!("{letter}").purple();
        }
        _ => format!("{letter}").white()
    }
}


impl Scrabble {

    pub fn new() -> Scrabble {
        let board = Board::new(Letter { letter: '#', value: 0 }, 15, 15);
        let rotated_board = board.rotate_board();

        return Scrabble { board,  rotated_board };
    }

    pub fn from(file: &str) -> Scrabble {
        let mut board = Board::new(Letter { letter: '#', value: 0 }, 15, 15);

        let content = fs::read_to_string(file).unwrap();
        let mut tracker = 0;
        for c in content.to_uppercase().chars() {
            match c {
                'A'..='Z' => {
                    board.board[tracker].letter = c;
                    board.board[tracker].value = LETTTERS_VALUES[c as usize - 'A' as usize];
                    tracker += 1;
                },
                '#' => {
                    board.board[tracker].letter = c;
                    board.board[tracker].value = 0;
                    tracker += 1;
                }
                _ => {}
            }
        };

        let rotated_board = board.rotate_board();
        return Scrabble {board, rotated_board};
    }

    pub fn print(&self) {
        print!("\t");
        for i in 0..self.board.width {
            print!("{i} ");
        }
        println!("");

        for j in 0..self.board.height {
            print!("{j}\t");
            for i in 0..self.board.width {
                print!("{} ", get_color(self.board[j][i].letter, j, i));
            }
            println!("");
        }
    }

    fn pattern(&self, groups: &Vec<Vec<LetterAndRange>>, spaces: &Vec<Vec<usize>>) -> Vec<Vec<Pattern>> {
        let mut ret: Vec<Vec<Pattern>> = Vec::with_capacity(self.board.height + self.board.width);

        for i in 0..self.board.height + self.rotated_board.height {
            ret.push(build_pattern(&groups[i], &spaces[i]));
        }

        return ret;
    }

    fn groups(&self) -> Vec<Vec<LetterAndRange>> {
        let mut ret: Vec<Vec<LetterAndRange>> = Vec::with_capacity(self.board.width + self.board.height);

        for i in 0..self.board.height {
            ret.push(build_groups(&self.board[i]));
        }

        for i in 0..self.rotated_board.height {
            ret.push(build_groups(&self.rotated_board[i]));
        }

        return ret;
    }

    fn spaces(&self, groups: &Vec<Vec<LetterAndRange>>) -> Vec<Vec<usize>> {
        let mut ret = Vec::with_capacity(self.board.width + self.board.height);

        for i in 0..self.board.height + self.rotated_board.height {
            ret.push(build_spaces(&groups[i], self.board.width))
        }

        return ret;
    }

    fn letters(&self, groups: &Vec<Vec<LetterAndRange>>) -> Vec<Vec<String>> {
        let mut ret = Vec::with_capacity(self.board.height + self.board.width);

        for i in 0..self.board.height + self.rotated_board.height {
            ret.push(build_letters(&groups[i]));
        }

        return ret;
    }

    fn ancors(&self) -> Vec<Vec<StartEnd>> {
        let mut ret = Vec::with_capacity(self.board.height + self.board.width);

        ret.push(get_ancor_edge(&self.board[0], &self.board[1]));
        for i in 1..self.board.height - 1 {
            ret.push(get_ancor(&self.board[i], &self.board[i - 1], &self.board[i + 1]))
        }
        ret.push(get_ancor_edge(&self.board[self.board.height - 1], &self.board[self.board.height - 2]));

        ret.push(get_ancor_edge(&self.rotated_board[0], &self.rotated_board[1]));
        for i in 1..self.rotated_board.height - 1 {
            ret.push(get_ancor(&self.rotated_board[i], &self.rotated_board[i - 1], &self.rotated_board[i + 1]));
        }
        ret.push(get_ancor_edge(&self.rotated_board[self.rotated_board.height - 1], &self.rotated_board[self.rotated_board.height - 2]));

        return ret;
    }


    pub fn moves(&self, letters: &str, joker_count: usize, repertoire: &Repertoire) -> Vec<usize> {

        let groups = self.groups();
        let spaces = self.spaces(&groups);
        let pattern = self.pattern(&groups, &spaces);
        let letters_on_board = self.letters(&groups);
        let ancors = self.ancors();

        let words = repertoire.filter(letters, joker_count);
        let words: Vec<&String> = words.iter().map(|i| &repertoire[*i]).collect();

        let mut out = Vec::new();

        for i in 0..self.board.height {
            find_best_word(letters, joker_count, repertoire, &pattern[i], &letters_on_board[i], &self.board, &self.rotated_board, &groups[i], i, false, &mut out);
            find_best_word(letters, joker_count, repertoire, &pattern[i + self.board.height], &letters_on_board[i + self.board.height], &self.rotated_board, &self.board, &groups[i + self.board.height], i, true, &mut out);
        }

        for i in 0..self.board.width {
            find_best_word_ancors(&self.board[i], &self.rotated_board, &groups[i], &ancors[i], &words, i, false, repertoire, &mut out);
            find_best_word_ancors(&self.rotated_board[i], &self.board, &groups[i + self.board.width], &ancors[i + self.board.width], &words, i, true, repertoire, &mut out);
        }

        out.sort_unstable_by(|a, b| b.word.point.partial_cmp(&a.word.point).unwrap());

        for i in &out {
            println!("{:?}", i);
        }

        return vec![joker_count; joker_count];
    }
}
