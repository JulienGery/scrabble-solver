pub struct Pattern {
    pattern: Vec<u8>,
}

fn does_match_chars(chars: &Vec<char>, index_in_word: usize, test: &[u8]) -> bool {
    if index_in_word + test.len() > chars.len() {
        // println!("index_in_word: {index_in_word}");
        return false;
    }

    for i in 0..test.len() {
        if chars[index_in_word + i] as u8 != test[i] {
            return false;
        }
    }

    return true;
}


fn does_match_pattern(chars: &Vec<char>, start_index: usize, patterns: &[u8]) -> bool {
    let mut index_in_word = start_index;
    let mut index_in_pattern: usize = 0;

    while index_in_pattern + 2 < patterns.len() {
        // println!("patterns len: {}", patterns.len());
        index_in_word += patterns[index_in_pattern] as usize;

        let chars_count = patterns[index_in_pattern + 1] as usize;
        if !does_match_chars(chars, index_in_word, &patterns[index_in_pattern + 2..index_in_pattern + 2 + chars_count]) {
            return false;
        }
        index_in_pattern += 2 + chars_count;
        index_in_word += chars_count;
    }

    let last_offset = *patterns.last().unwrap() as usize;

    if chars.len() - index_in_word > last_offset {
        return false
    }

    return true;
}

// relative_offset then number of chars then chars and so on.
impl Pattern {
    pub fn new() -> Pattern {
        return Pattern { pattern: vec![] };
    }

    pub fn with_capacity(capacity: usize) -> Pattern {
        return Pattern { pattern: Vec::with_capacity(capacity) };
    }

    pub fn push(&mut self, relative_offset: u8, chars: &str) {
        self.pattern.push(relative_offset);

        if !chars.is_empty() {
            self.pattern.push(chars.len() as u8);
            for i in chars.bytes() {
                self.pattern.push(i);
            }
        }
    }

    pub fn does_match(&self, chars: &Vec<char>) -> Option<Vec<usize>> {
        if self.pattern.len() == 0 {
            panic!("no pattern");
        }

        // let mut min_size = self.pattern[0].chars.len();
        let mut min_size = self.pattern[1] as usize;
        // let mut max_size = self.pattern[0].relative_offset + self.pattern[0].chars.len();
        let mut max_size = min_size + self.pattern[0] as usize;
        let mut current_index = 2 + min_size;

        while current_index < self.pattern.len() - 2 {
            min_size += self.pattern[current_index] as usize;
            max_size += self.pattern[current_index] as usize;

            min_size += self.pattern[current_index + 1] as usize;
            max_size += self.pattern[current_index + 1] as usize;
            current_index += 2 + self.pattern[current_index + 1] as usize;

        }
        max_size += self.pattern[current_index] as usize;

        // println!("min_size: {min_size}\tmax_size: {max_size}");

        if chars.len() < min_size || chars.len() > max_size {
            return None;
        }

        let mut ret = Vec::new();

        for i in 0..=self.pattern[0] as usize {
            if does_match_chars(&chars, i, &self.pattern[2..2 + self.pattern[1] as usize]) && does_match_pattern(&chars, i + self.pattern[1] as usize, &self.pattern[2 + self.pattern[1] as usize..]) {
                ret.push(i);
            }
        }


        if ret.len() == 0 {
            return None;
        }

        return Some(ret);
    }
}

