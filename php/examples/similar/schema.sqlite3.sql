CREATE TABLE pictures (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  digest CHAR(32) NOT NULL
);
CREATE TABLE sentpictures (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  url VARCHAR(255) NOT NULL,
  sender VARCHAR(100) NOT NULL,  
  picture_id INTEGER NOT NULL
);
CREATE TABLE signatures (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  compressed_signature CHAR(182) NOT NULL,
  picture_id INTEGER NOT NULL
);
CREATE TABLE words (
  pos_and_word CHAR(5) NOT NULL,
  signature_id INTEGER NOT NULL
);
CREATE UNIQUE INDEX idx_digest ON pictures(digest);
CREATE INDEX idx_picture_id ON sentpictures (picture_id);
CREATE INDEX idx_pos_and_word ON words(pos_and_word);
CREATE UNIQUE INDEX idx_url ON sentpictures (url);
