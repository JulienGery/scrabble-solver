use std::{
    fs,
    ops,
    collections::hash_set::HashSet,
};
use crate::pattern::Pattern;

pub struct Repertoire {
    words: Vec<String>,
    irs: Vec<u32>,
    letters_bags: Vec<u8>,
    words_map: HashSet<String>,
}

pub struct RegexResult {
    pub locations: Vec<usize>,
    pub case: usize,
}

pub struct WordsAndRegexResult {
    pub words: Vec<usize>,
    pub regex_resultes: Vec<RegexResult>,
}

fn lines_from_file(filename: &str) -> Vec<String> {
    let content = fs::read_to_string(filename).unwrap();

    content.lines().map(|l| String::from(l)).collect()
}

fn get_ir(word: &str) -> u32 {
    let mut ir : u32 = 0;
    for i in word.chars() {
        ir |= 0x80000000 >> (i as u8 - 'A' as u8);
    }
    return ir;
}

fn get_letters(letters :&str, letters_bag: &mut [u8]) {
    for i in letters.chars() {
        letters_bag[i as usize - 'A' as usize] += 1;
    }
}

fn get_letters_other(letters: &str, other_letters: &Vec<String>, start: usize, size: usize, letters_bag: &mut [u8])
{
    get_letters(letters, letters_bag);
    for i in 0..size {
        get_letters(&other_letters[i + start], letters_bag);
    }
}

fn sizes_helper(letters_on_board: &Vec<String>, start: usize, size: usize) -> usize {
    let mut ret = 0;
    for i in 0..size {
        ret += letters_on_board[i + start].len();
    }

    return ret;
}

fn is_feasible(test :&[u8], against: &[u8], mut jocker_count : i16) -> bool {
    for i in 0..26 {
        let diff = test[i] as i16 - against[i] as i16;
        if diff < 0 {
            jocker_count += diff;
            if jocker_count < 0 {
                return false;
            }
        }
    }

    return true;
}


impl ops::Index<usize> for Repertoire {
    type Output = String;

    fn index(&self, index: usize) -> &Self::Output {
        return &self.words[index];
    }
}


impl Repertoire {
    pub fn new(file: &str) -> Repertoire {
        let words = lines_from_file(file);
        let irs : Vec<_> = words.iter().map(|word| get_ir(word)).collect();
        let mut letters_bags : Vec<u8> = vec![0; words.len() * 26];

        for i in 0..words.len() {
            get_letters(&words[i], &mut letters_bags[26 * i..26 * (i + 1)])
        }

        let mut words_map = HashSet::with_capacity(words.len());

        for i in &words {
            words_map.insert(i.clone());
        }

        return Repertoire {
            words,
            irs,
            letters_bags,
            words_map
        }
    }

    pub fn valide(&self, word: &str) -> bool {
        if word.len() <= 1 {
            return true;
        }
        if self.words_map.contains(word) {
            return true;
        }
        return false;
    }


    pub fn filter(&self, letters : &str, jocker_count : usize) -> Vec<usize> {
        let mut ret : Vec<usize> = Vec::with_capacity(self.words.len());

        let letter_ir = get_ir(letters);
        let letter_count = letters.len() + jocker_count;

        if jocker_count == 0 {
            for i in 0..self.words.len() {
                if !(!self.irs[i] | letter_ir) == 0 && letter_count >= self.words[i].len() {
                    ret.push(i);
                }
            }
        } else {
            for i in 0..self.words.len() {
                if letter_count >= self.words[i].len() {
                    let mut tmp : u32 = !(!self.irs[i] | letter_ir);

                    for _ in 0..jocker_count {
                        tmp &= tmp - 1;
                    }

                    if tmp == 0 {
                        ret.push(i);
                    }
                }
            }
        }

        let mut letters_bag : [u8; 26] = [0; 26];
        get_letters(letters, &mut letters_bag);
        let mut valide = 0;
        for i in 0..ret.len() {
            let index = ret[i];
            if is_feasible(&letters_bag, &self.letters_bags[index * 26..(index + 1) * 26], jocker_count as i16) {
                ret[valide] = index;
                valide += 1;
            }
        }

        ret.resize(valide, 0);
        return ret;
    }

    pub fn advance_filter(&self, letters: &str, jocker_count: usize, regexes: &Vec<Pattern>, letters_on_board: &Vec<String>) -> WordsAndRegexResult {

        let n = letters_on_board.len();
        let mut sizes = vec![0; n * (n + 1) / 2];
        let mut letters_bags: Vec<u8> = vec![0; n * (n + 1) / 2 * 26];
        let mut track = 0;
        for i in (1..=n).rev() {
            for j in 0..=n - i {
                get_letters_other(letters, letters_on_board, j, i, &mut letters_bags[track * 26..(track + 1) * 26]);
                sizes[track] = sizes_helper(letters_on_board, j, i);
                track += 1;
            }
        }


        let mut all_letters: String = String::from(letters);
        for i in letters_on_board {
            all_letters += i;
        }

        let letter_count = all_letters.len() + jocker_count;
        let letter_ir = get_ir(&all_letters);

        let mut ret: Vec<usize> = Vec::with_capacity(self.words.len());

        if jocker_count == 0 {
            for i in 0..self.words.len() {
                if !(!self.irs[i] | letter_ir) == 0 && letter_count >= self.words[i].len() {
                    ret.push(i);
                }
            }
        } else {
            for i in 0..self.words.len() {
                if letter_count >= self.words[i].len() {
                    let mut tmp : u32 = !(!self.irs[i] | letter_ir);

                    for _ in 0..jocker_count {
                        tmp &= tmp - 1;
                    }

                    if tmp == 0 {
                        ret.push(i);
                    }
                }
            }
        }

        let mut valide = 0;
        let mut regex_resultes = Vec::with_capacity(ret.len());
        for i in 0..ret.len() {
            let index = ret[i];
            for j in 0..regexes.len() {
                if self.words[index].len() > sizes[j] && is_feasible(&letters_bags[j * 26..(j + 1) * 26], &self.letters_bags[index * 26..(index + 1) * 26], jocker_count as i16) {
                    if let Some(locations) = regexes[j].does_match(&self.words[index].chars().collect()) {
                        regex_resultes.push(RegexResult { locations, case: j });
                        ret[valide] = index;
                        valide += 1;
                        break;
                    }
                }
            }
        }

        ret.resize(valide, 0);
        return WordsAndRegexResult{words: ret, regex_resultes };
    }
}
