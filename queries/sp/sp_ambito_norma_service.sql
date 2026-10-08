DELIMITER //


CREATE PROCEDURE sp_insertar_ambito_norma(
    IN p_descripcion varchar(101)
)
BEGIN
    INSERT INTO ambito_norma (descripcion)
    VALUES (p_descripcion);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_ambito_norma(
    IN p_id INT,
    IN p_descripcion varchar(101)
)
BEGIN
    UPDATE ambito_norma
    SET descripcion = p_descripcion
    WHERE id = p_id;
END //

DELIMITER ;
