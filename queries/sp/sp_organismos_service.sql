DELIMITER //


CREATE PROCEDURE sp_insertar_organismo(
    IN p_cod_org VARCHAR(2),
    IN d_saf VARCHAR(4),
    IN p_descripcion varchar(300),
    IN p_ubicacion_fisica VARCHAR(120)
)
BEGIN
    INSERT INTO organismos (cod_org, saf, descripcion, ubicacion_fisica)
    VALUES (p_cod_org, p_saf, p_descripcion, p_ubicacion_fisica);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_organismo(
    IN p_id INT,
    IN p_cod_org VARCHAR(2),
    IN d_saf VARCHAR(4),
    IN p_descripcion varchar(300),
    IN p_ubicacion_fisica VARCHAR(120)
)
BEGIN
    UPDATE organismos
    SET cod_org = p_cod_org,
        saf = p_saf,
        descripcion = p_descripcion,
        ubicacion_fisica = p_ubicacion_fisica
    WHERE id = p_id;
END //

DELIMITER ;
