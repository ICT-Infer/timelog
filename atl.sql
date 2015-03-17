CREATE TABLE atl_users (
    id integer,
    username char(255) NOT NULL,
    full_name char(255),
    PRIMARY KEY (id)
);

CREATE TABLE atl_projects (
    id integer,
    parent integer,
    name char(255),
    description text,
    PRIMARY KEY (id),
    FOREIGN KEY (parent) REFERENCES atl_projects(id)
);

CREATE TABLE atl_entries (
    id integer NOT NULL,
    atl_user integer,
    atl_project integer,
    t_begin timestamp with time zone,
    t_end timestamp with time zone,
    description text,
    PRIMARY KEY (id),
    FOREIGN KEY (atl_user) REFERENCES atl_users(id),
    FOREIGN KEY (atl_project) REFERENCES atl_projects(id)
);
