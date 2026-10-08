DELIMITER //


CREATE PROCEDURE sp_insertar_tipo_organismo(
    IN p_cod_organismo varchar(2),
    IN p_descripcion varchar(120)
)
BEGIN
    INSERT INTO tipo_organismo (cod_organismo, descripcion)
    VALUES (p_cod_organismo, p_descripcion);
    SELECT LAST_INSERT_ID() AS nuevo_id;
END //

CREATE PROCEDURE sp_editar_tipo_organismo(
    IN p_id INT,
    IN p_cod_organismo varchar(2),
    IN p_descripcion varchar(120)
)
BEGIN
    UPDATE tipo_organismo
    SET cod_organismo = p_cod_organismo, descripcion = p_descripcion
    WHERE id = p_id;
END //

DELIMITER ;
