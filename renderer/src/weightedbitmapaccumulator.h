#ifndef WEIGHTEDBITMAPACCUMULATOR_H
#define WEIGHTEDBITMAPACCUMULATOR_H

#include "framebuffer.h"

namespace EARS {

struct WeightedBitmapAccumulator {
    void clear() {
        m_scrap = nullptr;
        m_bitmap = nullptr;
        m_spp = 0;
        m_weight = 0;
    }

    bool hasData() const {
        return m_weight > 0;
    }

    void add(Film &film, int spp, float avgVariance = 1) {
        if (avgVariance == 0 && m_weight > 0) {
            printf("Cannot add an image with unknown variance to an already populated accumulator\n");
            return;
        }

        const Vec2i size = film.size();
        const long floatCount = size.x() * size.y() * long(3);

        if (!m_scrap) {
            m_scrap = new Film(film.size());
        }
        for (int i = 0; i < film.buffer.size(); i++) {
            m_scrap->buffer[i] = film.buffer[i];
        }

        ///

        if (!m_bitmap) {
            m_bitmap = new Film(film.size());
            for (int i = 0; i < film.buffer.size(); i++) {
                m_bitmap->buffer[i] = Vec3f(0.0f);
            }
        }

        if (avgVariance > 0 && m_weight == 0 && m_spp > 0) {
            const float reweight = 1 / avgVariance;
            for (int i = 0; i < m_bitmap->buffer.size(); i++) {
                m_bitmap->buffer[i] *= reweight;
            }
            m_weight += m_spp * reweight;
        }

        const float weight = avgVariance > 0 ? spp / avgVariance : spp;
        for (int i = 0; i < m_bitmap->buffer.size(); i++) {
            m_bitmap->buffer[i] += m_scrap->buffer[i] * weight;
        }

        m_weight += avgVariance > 0 ? weight : 0;
        m_spp += spp;
    }

    void develop(Film *dest) const {
        if (!m_bitmap) {
            printf("Cannot develop bitmap, as no data is available\n");
            return;
        }

        const Vec2i size = m_bitmap->size();

        const float weight = m_weight == 0 ? m_spp : m_weight;
        for (int i = 0; i < dest->buffer.size(); i++) {
            dest->buffer[i] = weight > 0.0f ? m_bitmap->buffer[i] / weight : Vec3f(0.0f);
        }
    }

private:
    Film* m_scrap;
    Film* m_bitmap;
    float m_weight;
    int m_spp;
};

};

#endif //WEIGHTEDBITMAPACCUMULATOR_H
