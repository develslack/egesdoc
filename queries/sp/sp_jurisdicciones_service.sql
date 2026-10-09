DELIMITER //


CREATE PROCEDURE sp_insertar_jurisdiccion(
    IN p_cod_jur varchar(2),
    IN p_descripcion varchar(90)
)
BEGIN
    INSERT INTO jurisdicciones (cod_jur, descripcion)
    VALUES (p_cod_jur, p_descripcion);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_jurisdiccion(
    IN p_id INT,
    IN p_cod_jur varchar(2),
    IN p_descripcion varchar(90)
)
BEGIN
    UPDATE jurisdicciones
    SET cod_jur = p_cod_jur, descripcion = p_descripcion
    WHERE id = p_id;
END //

DELIMITER ;
