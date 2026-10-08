DELIMITER //


CREATE PROCEDURE sp_insertar_tipo_norma(
    IN p_descripcion varchar(101)
)
BEGIN
    INSERT INTO tipo_norma (descripcion)
    VALUES (p_descripcion);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_tipo_norma(
    IN p_id INT,
    IN p_descripcion varchar(101)
)
BEGIN
    UPDATE tipo_norma
    SET descripcion = p_descripcion
    WHERE id = p_id;
END //

DELIMITER ;
