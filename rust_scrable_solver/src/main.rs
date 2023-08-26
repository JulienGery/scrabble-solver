mod repertoire;
mod scrabble;
mod timer;
mod board;
mod pattern;

fn main() {
    {
        let args: Vec<String> = std::env::args().collect();
        let _timer = timer::Timer::new("total");
        // let letters = "XAVIER";
        let letters = &args[1];
        let jocker_count = 0;

        let repertoire;
        {
            let _timer = timer::Timer::new("build dictionary");
            repertoire = repertoire::Repertoire::new("French ODS dictionary.txt");
        }

        let scrabble = scrabble::Scrabble::from("boardInput.txt");
        scrabble.print();
        {
            let _timer = timer::Timer::new("find best word");
            scrabble.find_best_word(letters, jocker_count, &repertoire);
        }
    }
}
