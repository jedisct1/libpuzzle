--
-- PostgreSQL database dump
--

SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET SESSION AUTHORIZATION 'similar';

--
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: similar
--

COMMENT ON SCHEMA public IS 'Standard public schema';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: pictures; Type: TABLE; Schema: public; Owner: similar; Tablespace: 
--

CREATE TABLE pictures (
    id integer NOT NULL,
    digest character(32) NOT NULL,
    CONSTRAINT ck_digest CHECK ((char_length(digest) = 32))
);


--
-- Name: pictures_id_seq; Type: SEQUENCE; Schema: public; Owner: similar
--

CREATE SEQUENCE pictures_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: pictures_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: similar
--

ALTER SEQUENCE pictures_id_seq OWNED BY pictures.id;


--
-- Name: sentpictures; Type: TABLE; Schema: public; Owner: similar; Tablespace: 
--

CREATE TABLE sentpictures (
    id integer NOT NULL,
    url character varying(255) NOT NULL,
    sender character varying(100) NOT NULL,
    picture_id integer NOT NULL,
    CONSTRAINT ck_url CHECK (((url)::text <> ''::text))
);


--
-- Name: sentpictures_id_seq; Type: SEQUENCE; Schema: public; Owner: similar
--

CREATE SEQUENCE sentpictures_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: sentpictures_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: similar
--

ALTER SEQUENCE sentpictures_id_seq OWNED BY sentpictures.id;


--
-- Name: signatures; Type: TABLE; Schema: public; Owner: similar; Tablespace: 
--

CREATE TABLE signatures (
    id integer NOT NULL,
    compressed_signature bytea NOT NULL,
    picture_id integer NOT NULL,
    CONSTRAINT ck_signature CHECK ((octet_length(compressed_signature) >= 182))
);


--
-- Name: signatures_id_seq; Type: SEQUENCE; Schema: public; Owner: similar
--

CREATE SEQUENCE signatures_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: signatures_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: similar
--

ALTER SEQUENCE signatures_id_seq OWNED BY signatures.id;


--
-- Name: words; Type: TABLE; Schema: public; Owner: similar; Tablespace: 
--

CREATE TABLE words (
    pos_and_word bytea NOT NULL,
    signature_id integer NOT NULL,
    CONSTRAINT ck_pos_and_word CHECK ((octet_length(pos_and_word) >= 2))
);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: similar
--

ALTER TABLE pictures ALTER COLUMN id SET DEFAULT nextval('pictures_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: similar
--

ALTER TABLE sentpictures ALTER COLUMN id SET DEFAULT nextval('sentpictures_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: similar
--

ALTER TABLE signatures ALTER COLUMN id SET DEFAULT nextval('signatures_id_seq'::regclass);


--
-- Name: pictures_pkey; Type: CONSTRAINT; Schema: public; Owner: similar; Tablespace: 
--

ALTER TABLE ONLY pictures
    ADD CONSTRAINT pictures_pkey PRIMARY KEY (id);


--
-- Name: sentpictures_pkey; Type: CONSTRAINT; Schema: public; Owner: similar; Tablespace: 
--

ALTER TABLE ONLY sentpictures
    ADD CONSTRAINT sentpictures_pkey PRIMARY KEY (id);


--
-- Name: signatures_pkey; Type: CONSTRAINT; Schema: public; Owner: similar; Tablespace: 
--

ALTER TABLE ONLY signatures
    ADD CONSTRAINT signatures_pkey PRIMARY KEY (id);


--
-- Name: idx_digest; Type: INDEX; Schema: public; Owner: similar; Tablespace: 
--

CREATE UNIQUE INDEX idx_digest ON pictures USING btree (digest);


--
-- Name: idx_picture_id; Type: INDEX; Schema: public; Owner: similar; Tablespace: 
--

CREATE INDEX idx_picture_id ON sentpictures USING btree (picture_id);


--
-- Name: idx_pos_and_word; Type: INDEX; Schema: public; Owner: similar; Tablespace: 
--

CREATE INDEX idx_pos_and_word ON words USING btree (pos_and_word);


--
-- Name: idx_url; Type: INDEX; Schema: public; Owner: similar; Tablespace: 
--

CREATE UNIQUE INDEX idx_url ON sentpictures USING btree (url);


--
-- Name: sentpictures_picture_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: similar
--

ALTER TABLE ONLY sentpictures
    ADD CONSTRAINT sentpictures_picture_id_fkey FOREIGN KEY (picture_id) REFERENCES pictures(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: signatures_picture_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: similar
--

ALTER TABLE ONLY signatures
    ADD CONSTRAINT signatures_picture_id_fkey FOREIGN KEY (picture_id) REFERENCES pictures(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: words_signature_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: similar
--

ALTER TABLE ONLY words
    ADD CONSTRAINT words_signature_id_fkey FOREIGN KEY (signature_id) REFERENCES signatures(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

